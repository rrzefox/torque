#include <iostream>
#include <string>
#include <vector>
#include <errno.h>
#include "pbs_config.h"

#ifdef PENABLE_LINUX_CGROUPS
#include "machine.hpp"
#include <hwloc.h>

#include "pbs_error.h"
#include "log.h"
#include "utils.h"

using namespace std;

#define INTEL 1
#define AMD   2

const int CORE = 0;
const int THREAD = 1;

Core::Core() : id(-1), totalThreads(0), free(true), indices(), is_index_busy()
  {
  memset(core_cpuset_string, 0, MAX_CPUSET_SIZE);
  memset(core_nodeset_string, 0, MAX_NODESET_SIZE);
  }

Core::~Core()
  {
  id = -1;
  }


int Core::initializeCore(hwloc_obj_t core_obj, hwloc_topology_t topology)
  {
  /* We now need to find all of the processing units associated with this core */
  this->id = core_obj->logical_index;
  this->core_cpuset = core_obj->allowed_cpuset;
  this->core_nodeset = core_obj->allowed_nodeset;
  hwloc_bitmap_list_snprintf(this->core_cpuset_string, MAX_CPUSET_SIZE, this->core_cpuset);
  hwloc_bitmap_list_snprintf(this->core_nodeset_string, MAX_NODESET_SIZE, this->core_nodeset);
  translate_range_string_to_vector(this->core_cpuset_string, this->indices);

  // Add one of these for each index
  for (unsigned int i = 0; i < this->indices.size(); i++)
    this->is_index_busy.push_back(false);
  
  this->totalThreads = hwloc_get_nbobjs_inside_cpuset_by_type(topology, this->core_cpuset, HWLOC_OBJ_PU);
  this->processing_units_open = this->totalThreads;
  return(PBSE_NONE);
  }


void Core::mark_as_busy(
    
  int index)

  {
  for (unsigned int i = 0; i < this->indices.size(); i++)
    {
    if (this->indices[i] == index)
      {
      this->is_index_busy[i] = true;
      this->free = false;
      this->processing_units_open--;
      }
    }
  }


int Core::get_open_processing_unit()

  {
  int open_index = -1;

  for (unsigned int i = 0; i < this->is_index_busy.size(); i++)
    {
    if (this->is_index_busy[i] == false)
      {
      this->is_index_busy[i] = true;
      open_index = this->indices[i];
      this->processing_units_open--;
      break;
      }
    }

  this->free = false;

  return(open_index);
  } // END get_open_processing_unit()



/*
 * free_pu_index()
 *
 * Marks the processing unit with os index of index as free
 * Returns true if this os index is found
 *
 * @param index - the os index of the processing unit to mark as free
 * @return true if the index is part of this core
 */

bool Core::free_pu_index(

  int   index,
  bool &core_is_now_free)

  {
  core_is_now_free = false;

  for (unsigned int i = 0; i < this->indices.size(); i++)
    {
    if (this->indices[i] == index)
      {
      this->is_index_busy[i] = false;
      this->processing_units_open++;
      if (this->totalThreads == this->processing_units_open)
        {
        this->free = true;
        core_is_now_free = true;
        }

      return(true);
      }
    }

  return(false);
  } // END free_pu_index()



/*
 * add_processing_unit()
 * Adds the specified processing unit
 *
 * @param which - indicates if we're talking about a core or a thread
 * @param os_index - the operating system's index for this processing unit
 */

int Core::add_processing_unit(

  int which,
  int os_index)

  {
  if (which == CORE)
    {
    // We can't have more than 1 core
    if (this->id != -1)
      return(-1);

    this->id = os_index;
    }

  this->indices.push_back(os_index);
  this->is_index_busy.push_back(false);
  this->processing_units_open++;
  this->totalThreads++;

  return(PBSE_NONE);
  } // END add_processing_unit()



void Core::displayAsString(

  stringstream &out) const

  {
  out << "      Core " << this->id << " (" << this->totalThreads << " threads)\n";
  } // END displayAsString()



int Core::getNumberOfProcessingUnits()
  {
  return(this->totalThreads);
  }

int Core::get_id() const
  {
  return(this->id);
  }

void Core::unit_test_init()

  {
  this->totalThreads = 2;
  this->is_index_busy.push_back(false);
  this->is_index_busy.push_back(false);
  this->indices.push_back(0);
  this->indices.push_back(8);
  this->processing_units_open = 2;
  this->free = true;
  }


#endif /* PENABLE_LINUX_CGROUPS */  

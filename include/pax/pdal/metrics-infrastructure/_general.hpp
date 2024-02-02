//	Copyright (c) 2014-2022, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once


namespace pax::metrics {

	using metrics_value_type	  = float;

}	// namespace pax::metrics



/**************************************************************************************************************

ToDo:
=====

– Incorporate it in raster metrics.
– Incorporate it in plot metrics. 


Types:
======

Filter implements the filter. Constructable via id string. 
Function wraps the statistics function used. Constructable via id string. 
Function_filter contains a Filter and a Function. Constructable via id string. 
metric_id_set( ... ) creates a vector of Function_filter from a bunch of [Function_filter] id strings. 
Point_aggregator aggregates z values in a way, so that all filters can get the set of the data that they specify. 


The metrics calculations overview
=================================

The main parts of this system are the Z_values_aggregator and the Filter_algorithm_wrapper. 

1. A bunch of Filter_algorithm_wrapper objects are created from strings that each specify a filter and an algorithm. 
2. The Z_values_aggregator is fed las points and accumulates them in such a way that the Filter_algorithm_wrapper
   can get data from it. 
3. The Filter_algorithm_wrapper object contains a Filter object, that specifies what data to get from 
   Z_values_aggregator, and an Algorithm_wrapper object, that calculates the apropriate metric. 

The Metric_id_converter takes a Nilsson level and a string formatted in the "old" way and returns an id string 
formatted in the new way. If the string was not recognised according to the "old" formatting it is only passed on 
as it is probably using the new formatting. 

 **************************************************************************************************************/

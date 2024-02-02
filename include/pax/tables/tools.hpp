//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <span>


namespace pax {

	/// Always returns true.
	/// Useful in calls with required predicate but you don't need one: use always_true. 
	static constexpr auto always_true = []( std::size_t ){ return true; };


	/// Stream the header items to out_ using col_mark_ as column deligneater. 
	///
	template< typename Out, typename T, std::size_t N, typename Ch >
	void stream_deligneated(
		Out							  & out_,
		const std::span< T, N >			items_, 
		const Ch						col_mark_
	) {
		if( items_.size() ) {
			auto itr				  = items_.begin();
			const auto end			  = items_.end();
			out_ << *itr;
			while( ++itr < end )		out_ << col_mark_ << *itr;
		}
	}

}	// namespace pax

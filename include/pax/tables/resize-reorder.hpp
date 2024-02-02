//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <span>
#include <cassert>
#include <algorithm>	// std::copy_n, std::fill_n


namespace pax {


	/// We have 2d table data stored as an array and we need to reorder the data when resizing.
	/// - Undefined behaviour if data_.size() < min( old_rows_, new_rows_ ) * max( old_cols_, new_cols_ ).
	/// - It requires the 2d data to be accessed row first.
	///	- If we add columns, we need to 1. resize the array and 2. run this.
	///	- If we remove columns, we need to 1. run this and 2. resize the array.
	/// - If we add or remove rows with no change to the columns, we only need to resize the array.
	template< typename T, std::size_t N >
	void resize_reorder_2d(
		std::span< T, N >	data_, 
		const std::size_t 	old_rows_, 
		const std::size_t 	old_cols_, 
		const std::size_t 	new_rows_,
		const std::size_t 	new_cols_
	) noexcept {
		// We do not need to copy more rows than min( old_rows_, new_rows_ ).
		const auto			rows = ( old_rows_ < new_rows_ ) ? old_rows_ : new_rows_;

		// Check that we have the elbow room we need to reorder stuff.
		// Check, even if old_cols_ == new_cols_ since the principle is important.
		assert( data_.size() >= rows * ( ( old_cols_ > new_cols_ ) ? old_cols_ : new_cols_ ) );

		// If old_cols_ == new_cols_ we don't need to do anything.
		if( ( old_cols_ == new_cols_ ) || ( data_.size() <= 1 ) )		return;
		
		// old_cols_ < new_cols_ (copy rows from last to first).
		if( old_cols_ < new_cols_ ) {
			T			  * srce = data_.data() + old_cols_ * rows;
			T			  * dest = data_.data() + new_cols_ * rows;
			const T		  * dend = data_.data();
			while( ( dest-= new_cols_ ) >= dend ) {
				srce	 -= old_cols_;
				std::copy_n( srce, old_cols_, dest );
				std::fill_n( dest + old_cols_, new_cols_ - old_cols_, T{} );
			}

		// old_cols_ > new_cols_ (copy rows from first to last).
		} else {
			T			  * srce = data_.data();
			T			  * dest = data_.data();
			const T		  * dend = dest + new_cols_ * rows;
			while( ( dest+= new_cols_ ) < dend ) {
				srce	 += old_cols_;
				std::copy_n( srce, new_cols_, dest );
			}
		}

		// Zero trailing values, if any. 
		if( data_.size() > rows*new_cols_ )
			std::fill_n( data_.data() + rows*new_cols_, data_.size() - rows*new_cols_, T{} );
	}



	/// We have 2d table data stored as an array and we need to remove a column. Return what sould be the new size. 
	/// - Undefined behaviour if data_.size() < rows_*cols_.
	/// - It requires the 2d data to be accessed row first.
	template< typename T, std::size_t N >
	std::size_t remove_column(
		std::span< T, N >	data_, 
		const std::size_t 	rows_, 
		const std::size_t 	cols_, 
		const std::size_t 	remove_col_
	) noexcept {
		assert( data_.size() == rows_*cols_ );

		if( ( remove_col_ < cols_ ) && ( rows_ > 0 ) ) {
			// We leave be the elements before the first column item to remove.
			const auto		new_cols = cols_ - 1;
			T			  * dest = data_.data() + remove_col_;
			T			  * srce = dest + 1;
			const T		  * send = dest + cols_ * ( rows_ - 1 );

			// Copy the rows from behind one column item to remove to just before the next. 
			while( srce < send ) {
				std::copy_n( srce, new_cols, dest );
				srce	 += cols_;
				dest	 += new_cols;
			}

			// Copy the elements behind the last column item to remove, the trailing elements of the last row. 
			std::copy_n( srce, new_cols - remove_col_, dest );

			return rows_*new_cols;
		}
		
		return rows_*cols_;
	}


}	// namespace pax

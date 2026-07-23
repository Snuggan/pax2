//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "point.hpp"
#include <cmath>		// std::fmod
#include <cassert>


namespace pax {
	
	/// Implements an object with two corner coordinates, a bounding box.
	/// It can have any rank you please, but two (and sometimes three) is probably the usual.
	/// It may be used as a simple bounding box, to check if points or circles/plots are inside. 
	/// It is a superclass for Box_indexer, below, a tool to convert coordinates to a pixel in a raster.
	template< arithmetic A, std::size_t N >			requires( is_static< N > )
	struct Box {
		static constexpr std::size_t 				rank		  = N;
		using 										Pt			  = Point< A, N >;
		using 										Base		  = std::array< Pt, 2 >;
		using 										value_type	  = Pt::value_type;

	private:
		std::array< Point< A, N >, 2 >				m_box{};

		static constexpr A align_( const A value_, const A factor_ ) noexcept {
			return factor_ ? ( factor_ * std::floor( value_ / factor_ ) ) : value_;
		}

		/// Returns the closest number less than or equal to value_ that is evenly divisible by actor_.
		static constexpr A align_le( const A value_, const A factor_ ) noexcept {
			const A temp = align_( value_, factor_ );
			return  temp - ( ( temp > value_ ) ? factor_ : A{} );
		}

		/// Returns the closest number greater than or equal to value_ that is evenly divisible by factor_.
		static constexpr A align_ge( const A value_, const A factor_ ) noexcept {
			const A temp = align_( value_, factor_ );
			return temp + ( ( temp < value_) ? factor_ : A{} );
		}
		
	public:
		constexpr Box()											  = default;
		constexpr Box( const Box & )							  = default;
		constexpr Box & operator=( const Box & )				  = default;

		constexpr Box( 
			const Point< A, N > & pt0_, 
			const Point< A, N > & pt1_ 
		) noexcept : m_box({ pax::min( pt0_, pt1_ ), pax::max( pt0_, pt1_ ) }) {}

		constexpr const Pt & min()									const noexcept	{	return m_box.front();			}
		constexpr const Pt & max()									const noexcept	{	return m_box.back();			}
		constexpr bool empty() 										const noexcept	{	return !all_lt( min(), max() );	}
		constexpr       Pt   sides()								const noexcept	{	return max() - min();			}
		friend constexpr const Pt & min( const Box & b_ )			noexcept		{	return b_.min();				}
		friend constexpr const Pt & max( const Box & b_ )			noexcept		{	return b_.max();				}
		friend constexpr bool empty( const Box & b_ )				noexcept		{	return b_.empty();				}

		friend constexpr bool operator==( const Box & b0_, const Box & b1_ ) noexcept {
			return ( b0_.min() == b1_.min() ) && ( b0_.max() == b1_.max() );
		}

		/// Returns the minimal Box that contains both the original Box and pt_.
		constexpr Box aligned( value_type resolution_ )				const noexcept	{
			resolution_			  = std::abs( resolution_ );
			auto [ ... small ]	  = min();
			auto [ ... large ]	  = max();
			return { Pt{ align_le( small, resolution_ ) ... }, Pt{ align_ge( large, resolution_ ) ... } };
		}

		/// Returns the minimal Box that contains both the original Box and pt_.
		constexpr Box grow( const Point< A, N > & pt_ )				const noexcept	{
			return { pax::min( min(), pt_ ), pax::max( max(), pt_ ) };
		}

		/// Is the point inside the Box, but not on its borders?
		constexpr bool strictly_inside( const Point< A, N > & pt_ )	const noexcept	{
			return all_lt( pt_, max() ) && all_lt( min(), pt_ );
		}

		/// Is the point inside the Box or touching its borders?
		constexpr bool inside_or_on( const Point< A, N > & pt_ )	const noexcept	{
			return all_le( pt_, max() ) && all_le( min(), pt_ );
		}

		/// Is the point inside the Box or touching its minimal (but not maximal) borders?
		constexpr bool in_range( const Point< A, N > & pt_ )		const noexcept	{
			return all_lt( pt_, max() ) && all_le( min(), pt_ );
		}

		/// Box contents to std::string.
		explicit constexpr operator std::string() 					const			{
			return std::format( "[{}, {}]", min(), max() );
		}

		/// Stream a box contents.
		template< typename Out >
		friend constexpr Out & operator<<(
			Out				  & out_,
			const Box		  & box_
		) {
			return out_ << std::string( box_ );
		}
	};
	
	using Box2d				  = Box< double, 2 >;
	using Box3d				  = Box< double, 3 >;

	template< arithmetic A, std::size_t N >
	Box( const Point< A, N > &, const Point< A, N > & ) -> Box< A, N >;




	/// Handles transformation of multiple indices (of matrix) to scalar index [into a vector]. 
	/// It can have any rank you please, but two (and sometimes three) is probably the usual.
	/// It may be used to handle multiple indeces, such as for rasters and multi-dimensional arrays.
	/// It is a superclass for Box_indexer, below, together with Box.
	template< std::size_t N >									requires( is_static< N > )
	struct Indexer {
		static constexpr std::size_t	rank				  = N;
		using 							Idx					  = Index< rank >;
		using 							index_type			  = Idx::value_type;

	private:
		Idx								m_size{}, m_offsets{};
		static constexpr Idx			noll{};
		
		static constexpr Idx do_offs( const Idx & idx_ )		noexcept	{
			index_type					product{ 1u };
			auto [ ... t, tn ]		  = idx_;
			return { product, ( product *= t ) ... };
		}
		
	public:
		constexpr Indexer()									  = default;
		constexpr Indexer( const Indexer & )				  = default;
		constexpr Indexer & operator=( const Indexer & )	  = default;

		constexpr Indexer( const Idx & size_ )  				noexcept : m_size( size_ ), m_offsets{ do_offs( size_ ) } {}

		constexpr const Idx & size()							const noexcept	{	return m_size;							}
		constexpr const Idx & offsets()							const noexcept	{	return m_offsets;						}
		constexpr index_type elements()							const noexcept	{	return offsets().back()*size().back();	}
		constexpr bool valid_index( const Idx & idx_ )			const noexcept	{	return all_lt( idx_, size() );			}
		friend constexpr index_type cols( const Indexer & i_ )		  noexcept	{	return col( i_.size() );				}
		friend constexpr index_type rows( const Indexer & i_ )		  noexcept	{	return row( i_.size() );				}
		
		/// Calculate an index into a vector for the index represented by pt_.
		template< uinteger ...U >								requires( sizeof...( U ) == N )
		constexpr index_type operator[]( U && ... u_ )			const noexcept	{
			return operator[]( Idx{ std::forward< U >( u_ ) ... } );
		}
		
		/// Calculate an index into a vector for the index represented by pt_.
		constexpr index_type operator[]( const Idx & idx_ )		const noexcept	{
			return dot_product( idx_, offsets() );
		}
		
		/// Calculate an index into a vector for the index represented by pt_.
		template< uinteger ...U >								requires( sizeof...( U ) == N )
		constexpr index_type at( U && ...u_ )					const noexcept	{
			return at( Idx{ std::forward< U >( u_ ) ... } );
		}
		
		/// Calculate an index into a vector for the index represented by pt_.
		constexpr index_type at( const Idx & idx_ )				const noexcept	{
			assert( all_lt( idx_, size() ) );
			return dot_product( idx_, offsets() );
		}

		/// Box contents to std::string.
		explicit constexpr operator std::string() 					const			{
			return std::format( "[{}]", size() );
		}

		/// Stream a box contents.
		template< typename Out >
		friend constexpr Out & operator<<(
			Out				  & out_,
			const Indexer	  & idxer_
		) {
			return out_ << std::string( idxer_ );
		}
	};
	
	using Indexer2d			  = Indexer< 2 >;
	using Indexer3d			  = Indexer< 3 >;

	template< std::size_t N >
	Indexer( const Index< N > & ) -> Indexer< N >;




	/// A bounding box that also handles coordinattes to scalar index transformation.
	/// - It can have any rank you please, but two (a raster) is probably the usual.
	/// - Rank two is handled differently, see comment on ::index( Pt ), below. 
	template< arithmetic A, std::size_t N >						requires( is_static< N > )
	struct Box_indexer : public Box< A, N >, public Indexer< N > {
		static constexpr std::size_t 		rank			  = N;
		using 								BBox			  = Box< A, rank >;
		using 								Pt				  = BBox::Pt;
		using 								value_type		  = Pt::value_type;
		using 								Idx				  = Indexer< rank >;
		using 								index_type		  = Idx::index_type;

	private:
		value_type									m_resolution{};

		static constexpr Point< std::size_t, N > do_idx(
			const Point< A, N >			  & sides_,
			const A							resolution_
		) noexcept {
			// We want no zero-length dimension.
			static constexpr auto mini	  = []( index_type i_ ) { return ( i_ > 1u ) ? i_ : 1u; };
			auto [ ... s ]				  = sides_;
			return { mini( s/resolution_ ) ... };
		}

	public:
		constexpr Box_indexer()									  = default;
		constexpr Box_indexer( const Box_indexer & )			  = default;
		constexpr Box_indexer & operator=( const Box_indexer & )  = default;

		constexpr Box_indexer(
			const BBox					  & box_, 
			const value_type				resolution_
		) noexcept : 
			BBox{ box_.aligned( resolution_ ) }, 
			Idx { do_idx( BBox::sides(), resolution_ ) }, 
			m_resolution{ resolution_ } 
		{}

		/// The size of the grid elements.
		constexpr value_type resolution()	const noexcept	{	return m_resolution;	}

		/// Given a point, what offset does it have into the vector of data?
		///	- If pt_ is outside the bounding box, an exception is thrown.
		/// - When dealing with rasters (rank is two), origo is at the upper left corner, which differs 
		///   from the mathematical origo (lower left corner). So if rank is two, this is taken care of. 
		index_type index( const Pt & pt_ )	const			{
			static constexpr auto smallest = []( value_type c, index_type i ){
				const auto ci = static_cast< index_type >( c );
				return ( ci < i ) ? ci : i;
			};
			if( BBox::inside_or_on( pt_ ) ) {
				if constexpr( rank == 2 ) {	// The exception of the raster case, mentioned above.
					return Idx::operator[](
						smallest( ( x( pt_ ) -  x( BBox::min() ))/resolution(), col( Idx::size() ) - 1u ),
						smallest( ( y( BBox::max() ) - y( pt_ ) )/resolution(), row( Idx::size() ) - 1u )
					);
				} else {					// The general case, with mathematical origo. 
					auto [ ... diff ]	  = pt_ - BBox::min();
					auto [ ... sz   ]	  = Idx ::size();
					return Idx::operator[]( { smallest( diff/resolution(), sz - 1 ) ... } );
				}
			} else throw std::runtime_error( 
				std::format( "The point {} is outside the bbox [{}, {}].", pt_, min( *this ), max( *this ) ) );
		}

		/// The affine transformation of the raster, as defined by gdal.
		constexpr auto affine_vector()		const noexcept		requires( rank == 2 )	{
			return std::array< double, 6 >{
	    		double( x( min( *this ) ) ),	double( resolution() ),	   double{},
				double( y( max( *this ) ) ),	double{},				( -resolution() )
			};
		}

		/// Box contents to std::string.
		explicit constexpr operator std::string() const {
			return std::format( "{{{}, {}}}", std::string( BBox( *this ) ), resolution() );
		}

		/// Stream a box contents.
		template< typename Out >
		friend constexpr Out & operator<<(
			Out					  & out_,
			const Box_indexer	  & boxi_
		) {
			return out_ << std::string( boxi_ );
		}
	};
	
	using Box_indexer2d		  = Box_indexer< double, 2 >;
	using Box_indexer3d		  = Box_indexer< double, 3 >;

	template< arithmetic A, std::size_t N, arithmetic A2 >
	Box_indexer( const Box< A, N > &, A2 ) -> Box_indexer< A, N >;

}	// namespace pax

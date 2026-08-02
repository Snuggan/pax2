//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "point.hpp"
#include <cmath>		// std::fma, std::floor, std::abs


namespace pax {
	
	/// Implements an object with two corner coordinates, a bounding box.
	/// It can have any rank you please, but two (and sometimes three) is probably the usual.
	/// It may be used as a simple bounding box, to check if points or circles/plots are inside. 
	/// It is a superclass for Box_indexer, below, a tool to convert coordinates to a pixel in a raster.
	template< floating F, std::size_t N >							requires( is_static< N > )
	struct Box {
		static constexpr std::size_t 		rank				  = N;
		using 								Pt					  = Point< F, N >;
		using 								Base				  = std::array< Pt, 2 >;
		using 								value_type			  = Pt::value_type;

	private:
		std::array< Point< F, N >, 2 >		m_box{};

		static constexpr F align_( const F value_, const F factor_ ) noexcept {
			return factor_ ? ( factor_ * std::floor( value_ / factor_ ) ) : value_;
		}

		/// Returns the closest number less than or equal to value_ that is evenly divisible by actor_.
		static constexpr F align_le( const F value_, const F factor_ ) noexcept {
			const F temp = align_( value_, factor_ );
			return  temp - ( ( temp > value_ ) ? factor_ : F{} );
		}

		/// Returns the closest number greater than or equal to value_ that is evenly divisible by factor_.
		static constexpr F align_ge( const F value_, const F factor_ ) noexcept {
			const F temp = align_( value_, factor_ );
			return  temp + ( ( temp < value_) ? factor_ : F{} );
		}
		
	public:
		constexpr Box()											  = default;
		constexpr Box( const Box & )							  = default;
		constexpr Box & operator=( const Box & )				  = default;

		constexpr Box( 
			const Point< F, N > & pt0_, 
			const Point< F, N > & pt1_ 
		) noexcept : m_box({ pax::min( pt0_, pt1_ ), pax::max( pt0_, pt1_ ) }) {}

		constexpr const Base & box()								const noexcept	{	return m_box;					}
		constexpr const Pt   & min()								const noexcept	{	return box().front();			}
		constexpr const Pt   & max()								const noexcept	{	return box().back();			}
		constexpr       bool   empty() 								const noexcept	{	return !all_lt( min(), max() );	}
		constexpr       Pt     sides()								const noexcept	{	return max() - min();			}

		friend constexpr const Pt & min( const Box & b_ )			noexcept		{	return b_.min();				}
		friend constexpr const Pt & max( const Box & b_ )			noexcept		{	return b_.max();				}
		friend constexpr bool  empty( const Box & b_ )				noexcept		{	return b_.empty();				}

		friend constexpr bool operator==( const Box & b0_, const Box & b1_ ) noexcept {
			return ( b0_.min() == b1_.min() ) && ( b0_.max() == b1_.max() );
		}

		/// Returns the minimal Box that contains both the original Box and pt_.
		constexpr Box aligned( Pt resolution_ )						const noexcept	{
			const auto [ ...   res ]	  = resolution_;
			const auto [ ... small ]	  = min();
			const auto [ ... large ]	  = max();
			return { { align_le( small, std::abs( res ) ) ... }, 
					 { align_ge( large, std::abs( res ) ) ... } };
		}

		/// Returns the minimal Box that contains both the original Box and pt_.
		constexpr Box aligned( const value_type resolution_ )		const noexcept	{
			return aligned( pax::point< rank >( resolution_ ) );
		}

		/// Returns the minimal Box that contains both the original Box and pt_.
		constexpr Box grow( const Point< F, N > & pt_ )				const noexcept	{
			return { pax::min( min(), pt_ ), pax::max( max(), pt_ ) };
		}

		/// Is the point inside the Box, but not on its borders?
		constexpr bool strictly_inside( const Point< F, N > & pt_ )	const noexcept	{
			return all_lt( pt_, max() ) && all_lt( min(), pt_ );
		}

		/// Is the point inside the Box or touching its borders?
		constexpr bool inside_or_on( const Point< F, N > & pt_ )	const noexcept	{
			return all_le( pt_, max() ) && all_le( min(), pt_ );
		}

		/// Is the point inside the Box or touching its minimal (but not maximal) borders?
		constexpr bool in_range( const Point< F, N > & pt_ )		const noexcept	{
			return all_lt( pt_, max() ) && all_le( min(), pt_ );
		}

		/// Box contents as a std::string.
		constexpr std::string string() 								const			{
			return std::format( "[{}, {}]", min(), max() );
		}
	};
	
	using Box2d							  = Box< double, 2 >;
	using Box3d							  = Box< double, 3 >;

	template< floating F, std::size_t N >
	Box( const Point< F, N > &, const Point< F, N > & ) -> Box< F, N >;




	/// Handles transformation of multiple indices (of matrix) to scalar index [into a vector]. 
	/// It can have any rank you please, but two (and sometimes three) is probably the usual.
	/// It may be used to handle multiple indeces, such as for rasters and multi-dimensional arrays.
	/// It is a superclass for Box_indexer, below, together with Box.
	template< std::size_t N >										requires( is_static< N > )
	struct Indexer {
		static constexpr std::size_t		rank				  = N;
		using 								Idx					  = Index< rank >;
		using 								index_type			  = Idx::value_type;

	private:
		Idx									m_extents{}, m_offsets{};
		static constexpr Idx				noll{};
		
		static constexpr Idx do_offs( const Idx & idx_ )			noexcept		{
			index_type						product{ 1u };
			auto [ ... t, tn ]			  = idx_;
			return { product, ( product *= t ) ... };
		}
		
	public:
		constexpr Indexer()										  = default;
		constexpr Indexer( const Indexer & )					  = default;
		constexpr Indexer & operator=( const Indexer & )		  = default;

		constexpr Indexer( const Idx & extents_ )  					noexcept 
			: m_extents( extents_ ), m_offsets{ do_offs( extents_ ) } {}

		/// Number of elementa in each dimension.
		constexpr const Idx & extents()								const noexcept	{	return m_extents;			}

		/// The stride between elements in each dimension. 
		constexpr const Idx & offsets()								const noexcept	{	return m_offsets;			}

		/// The total number of elements (product of all sizes).
		constexpr index_type elements()								const noexcept	{
			return offsets().back()*extents().back();
		}
		
		/// Returns true, iff all indeces in i_ are smaller the the eqivalent size. 
		constexpr bool valid_index( const Idx & idx_ )				const noexcept	{
			return all_lt( idx_, extents() );
		}
		
		/// The number of "columns", same as size()[ col_idx ].
		friend constexpr index_type cols( const Indexer & i_ )		noexcept		{	return col( i_.extents() );	}
		
		/// The number of "rows", same as size()[ row_idx ].
		friend constexpr index_type rows( const Indexer & i_ )		noexcept		{	return row( i_.extents() );	}
		
		/// Calculate an index into a vector for the index represented by pt_.
		template< uinteger ...U >									requires( sizeof...( U ) == N )
		constexpr index_type operator[]( U && ... u_ )				const noexcept	{
			return operator[]( Idx{ std::forward< U >( u_ ) ... } );
		}
		
		/// Calculate an index into a vector for the index represented by pt_.
		constexpr index_type operator[]( const Idx & idx_ )			const noexcept	{
			return dot_product( idx_, offsets() );
		}

		/// Indexer contents as a std::string.
		constexpr std::string string() 								const			{
			return std::format( "{}", extents() );
		}
	};
	
	using Indexer2d						  = Indexer< 2 >;
	using Indexer3d						  = Indexer< 3 >;

	template< std::size_t N >
	Indexer( const Index< N > & ) -> Indexer< N >;




	/// A bounding box that also handles coordinattes to scalar index transformation.
	/// - If you intend to use it with a [gdal] raster or pictures, you should most 
	///	  probably use Raster_indexer instead. 
	template< floating F, std::size_t N >						requires( is_static< N > )
	struct Box_indexer : public Box< F, N >, public Indexer< N > {
		static constexpr std::size_t 		rank			  = N;
		using 								BBox			  = Box< F, rank >;
		using 								Pt				  = BBox::Pt;
		using 								value_type		  = Pt::value_type;
		using 								Idx				  = Indexer< rank >;
		using 								index_type		  = Idx::index_type;

	protected:
		Pt									m_resolution{};		// The element size (all positive).
		Pt									m_factor{};			// Multiply a point with this...
		Pt									m_offset{};			// ...and add this to get the index. 

		static constexpr Point< std::size_t, N > do_idx(
			const Pt					  & sides_,
			const Pt					  & resolution_
		) noexcept {
			// We want no zero-length dimension.
			static constexpr auto mini	  = []( index_type i_ ) { return ( i_ > 1u ) ? i_ : 1u; };
			const auto [ ... side ]		  = sides_;
			const auto [ ...  res ]		  = resolution_;
			return { mini( side/res ) ... };
		}
		
		static constexpr auto smallest = []( value_type c, index_type i ){
			const auto ci = static_cast< index_type >( c );
			return ( ci < i ) ? ci : i;
		};


	public:
		using coord_type										  = value_type;

		constexpr Box_indexer()									  = default;
		constexpr Box_indexer( const Box_indexer & )			  = default;
		constexpr Box_indexer & operator=( const Box_indexer & )  = default;

		/// The main constructor that calculates the transformation attributes.
		constexpr Box_indexer(
			const BBox					  & box_, 
			const Pt					  & resolution_
		) : 
			BBox{ box_.aligned( resolution_ ) }, 
			Idx { do_idx( BBox::sides(), resolution_ ) }, 
			m_resolution( resolution_ )
		{
			if( !all_lt( Pt{}, resolution_ ) )	throw std::runtime_error( 
				std::format( "All resolutions must be positive, they are not: {}.", resolution_ ) );

			// Calculate the actual transformation attributes. 
			const auto [ ... min ]		  = BBox::min();
			const auto [ ... res ]		  = resolution();
			m_factor					  = {    1/res ... };
			m_offset					  = { -min/res ... };
		}

		/// Simplified constructor, when elements have the same length in all dimensions.
		constexpr Box_indexer(
			const BBox					  & box_, 
			const value_type				resolution_
		) : Box_indexer( box_, pax::point< rank >( resolution_ ) ) {}

		/// The size of the grid elements.
		constexpr Pt resolution()									const noexcept	{	return m_resolution;	}

		/// Easy access to the superclass.
		constexpr const BBox & box()								const noexcept	{	return *this;			}

		/// Given a point, what offset does it have into the vector of data?
		///	If pt_ is outside the bounding box the result is undefined. So unless you are sure it is not 
		/// outside, you shoud check this with either [Box_indexer::]in_range, strictly_inside, or inside_or_on.
		index_type index( const Pt & pt_ )							const			{
			const auto [ ...     pt ]	  = pt_;
			const auto [ ... factor ]	  = m_factor;
			const auto [ ... offset ]	  = m_offset;
			const auto [ ...   exts ]	  = Idx::extents();
			// The exts are necessary to include points with any coordinate value on the max edge:
			return Idx::operator[]( { smallest( std::fma( pt, factor, offset ), exts - 1 ) ... } );
		}
		
		/// Given an index, returns the coordinates of the element's lower left corner. 
		/// - Mainly used for debugging. 
		/// - Raster_indexer returns the coordinates for the element's upper left corner. 
		///	- If idx_ is >= elements(), the result is undefined. 
		///   So unless you are sure it is ok, you better check it with all_lt( idx_, extents() ). 
		constexpr Pt point( const Index< rank > & idx_ )			const noexcept	{
			const auto [ ...    idx ]	  = idx_;
			const auto [ ... factor ]	  = m_factor;
			const auto [ ... offset ]	  = m_offset;
			return { ( idx - offset )/factor ... };
		}

		/// Box contents as a std::string.
		constexpr std::string string() 								const			{
			return std::format( "{{{}, {}}}", box().string(), resolution() );
		}
	};
	
	using Box_indexer2d					  = Box_indexer< double, 2 >;
	using Box_indexer3d					  = Box_indexer< double, 3 >;

	template< floating F, std::size_t N, arithmetic F2 >
	Box_indexer( const Box< F, N > &, F2 ) -> Box_indexer< F, N >;

	template< floating F, std::size_t N >
	Box_indexer( const Box< F, N > &, Point< F, N > ) -> Box_indexer< F, N >;



	///	A special case of Box_indexer to use with [gdal] rasters.
	/// - "Pictures!" do not normally use the mathematical origin, but instead the upper left corner and go down. 
	/// - This class do not handle full affine transformations – no rotations or obliqueness. 
	struct Raster_indexer : public Box_indexer< double, 2 > {
		using Boxer = Box_indexer< double, 2 >;
		constexpr Raster_indexer()										  = default;
		constexpr Raster_indexer( const Raster_indexer & )				  = default;
		constexpr Raster_indexer & operator=( const Raster_indexer & )	  = default;

		/// The main constructor that calculates the transformation attributes.
		constexpr Raster_indexer(
			const BBox					  & box_, 
			const Pt					  & resolution_
		) : Boxer{ box_, resolution_ } {
			// Give values that will used a "downwards" system with origo at the upper left corner:
			Boxer::m_factor = {             1.0  /x( resolution() ),           -1.0  /y( resolution() ) };
			Boxer::m_offset = { -x( BBox::min() )/x( resolution() ), y( BBox::max() )/y( resolution() ) };
		}

		/// Simplified constructor, when elements have the same length in all dimensions.
		constexpr Raster_indexer( const BBox & box_, const value_type resolution_ ) 
			: Raster_indexer( box_, pax::point< 2 >( resolution_ ) ) {}

		/// Return the affine values, in order specified by gdal.
		constexpr Point< double, 6 > affine_values()				const noexcept	{
			return {
				x( this->min() ),	 x( Boxer::resolution() ),		double{},
				y( this->max() ),		double{},				-y( Boxer::resolution() )
			};
		}
	};

}	// namespace pax

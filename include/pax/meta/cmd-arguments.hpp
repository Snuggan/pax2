//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#pragma once

#include "cmd-arguments/arguments.hpp"
#include "cmd-arguments/terminal.hpp"
#include "cmd-arguments/parse_json.hpp"

#include <iostream>						// std::cout


namespace pax::cmd_args {
	
	class Parameter_type {
		enum class Type : uint8_t { on_flag, off_flag, scalar, zero_or_more, one_or_more };

		constexpr Parameter_type( Type prop_ )		  noexcept	:	m_type{ prop_ } {}

		Type									m_type;

		static constexpr std::size_t 			s_properies{ 5 };
		static_assert( s_properies == int( Type::one_or_more )+1 );
		static constexpr std::string_view		s_names[ s_properies ] = 
			{ "flag, default on", "flag, default off", "scalar value", "zero or more values", "one or more values" };
		static constexpr std::size_t			s_values_min[ s_properies ] = { 0u, 0u, 1u,  0u,  1u };
		static constexpr std::size_t			s_values_max[ s_properies ] = { 0u, 0u, 1u, -1u, -1u };
		
	public:
		constexpr Parameter_type() 					  noexcept	:	m_type{ Type::scalar } {}
		constexpr std::string_view meta()		const noexcept	{	return s_names[ unsigned( m_type ) ];			}

		constexpr Type type()					const noexcept	{	return m_type;									}
		constexpr bool is_on_flag()				const noexcept	{	return type() == Type::on_flag;					}
		constexpr bool is_off_flag()			const noexcept	{	return type() == Type::off_flag;				}
		constexpr bool is_flag()				const noexcept	{	return is_on_flag() || is_off_flag();			}
		constexpr bool is_multiple()			const noexcept	{	return s_values_max[ unsigned( type() ) ] > 1;	}

		static constexpr auto on_flag()				  noexcept	{	return Parameter_type{ Type::on_flag };			}
		static constexpr auto off_flag()			  noexcept	{	return Parameter_type{ Type::off_flag };		}
		static constexpr auto scalar()				  noexcept	{	return Parameter_type{ Type::scalar };			}
		static constexpr auto zero_or_more_values()	  noexcept	{	return Parameter_type{ Type::zero_or_more };	}
		static constexpr auto one_or_more_values()	  noexcept	{	return Parameter_type{ Type::one_or_more };		}

		/// Delimiter for delimiting is_multiple() argument values (--.multi==1,2,3). 
		template< Character Char >
		static constexpr auto					delimiter{ Char( ',' ) };
	};

	
	/// Class used to set the invisible attribute via the constructor.
	struct Invisible {};


	/// Class used to set the default value via the Param constructor.
	/** A parameter is 'required' iif it has no default value.					**/
	template< Character Char >
	struct Default_value {
		using String_view					  = std::basic_string_view< Char >;

		Default_value() 						noexcept : m_default( null_string ) {}
		Default_value( const String_view dv_ )			 : m_default( dv_ ) {}
		Default_value( const Char * ptr_ )				 : Default_value( String_view( ptr_ ) ) {}

		bool empty()							const noexcept	{	return String_view( value() ) == null_string;	};
		auto value()							const noexcept	{	return m_default;								};
		
	private:
		using String						  = std::basic_string< Char >;
		static constexpr String_view			null_string = "\n";
		String									m_default;
	};




	
	/// Creates a parameter ensamble and parses the argc/argv on the ensample. 
	/**	The main components of this code to store parameters and, based on them, produce accessible arguments 
		that are morfologically checked from textual arguments (argv/argc).
		- Parameter: defines the (long) name, an optional (character) abbrev., and behaviour -- but not value -- of the parameters.
		- Group: a group of zero or more Parameter, only used in the user interface. 
		- Parameter_set: a set of one or more Group. A group-less Parameter is handled by using an anonymous Group. 
		- Arguments: a set of values as the result of a Parameter_set successful parse of argv/argc. It is self-contained, 
		  so once you have it the Parameter_set may be dispoised of. 
		You define a Parameter set by stating what Parameters and Groups it contains. You parse it and use the resulting Arguments in your code. 
		Parameters and Groups can be definied individually and used by more than one Parameter_set. 
	**/
	template< Character Char >
	class Parameter_set {
		using Arguments_type			  = Arguments< Char >;
		using String					  = std::basic_string< Char >;
		static constexpr auto m_args_file = std::basic_string_view< Char >{ "args-file" };

	public:
		using String_view				  = std::basic_string_view< Char >;
		
		/// A parameter definition.
		class Parameter;
		
		/// Group parameters together.
		class Group;
		

		/// The default constructor adds a group with --version and --help parameters. 
		Parameter_set(
			const String_view 				version_		= "", 
			const String_view 				description_	= "", 
			const String_view 				usage_			= ""
		) : m_version( version_ ), 
			m_description( description_ ), 
			m_usage( usage_ )
		{
			*this << Group{}
				( 		id_anonymous,	"Used for catching unnamed parameters.",	Parameter_type::zero_or_more_values(), Invisible{}	)
				(		m_args_file,	"Json file[s] with additional cmd line arguments. " 
										"There may be more than one such file and they may be mixed with other arguments. "
										"The contents of them is inserted 'instead of' this argument, order-wise."
										"",											Parameter_type::zero_or_more_values()				)
				( 		"version",		"Output version information.",				Parameter_type::off_flag()							)
				( '?',	"help",			"Output usage information.",				Parameter_type::off_flag()							)
				;
		}
	
		template< typename Meta >
		Parameter_set( const Meta & meta_ ) : Parameter_set{ info( meta_ ), description( meta_ ), usage( meta_ ) } {}
	
		Parameter_set( const Parameter_set & )				= default;
		Parameter_set( Parameter_set && )					= default;
		Parameter_set & operator=( const Parameter_set & )	= default;
		Parameter_set & operator=( Parameter_set && )		= default;

		/// Parse argv_ and check that all looks ok.
		/** If any of version_, description_, or usage_ is non-empty:
			- The function will handle --help and --version and then terminate with EXIT_SUCCESS.
			- It will display any error message and then terminate with EXIT_FAILURE.
			Otherwise, any error will throw an exception.
		**/
		template< std::size_t N >
		Arguments_type parse( const std::span< const Char * const, N > args_ ) const;

		Arguments_type parse( const std::size_t argc_, const Char ** argv_ ) const {
			return parse( std::span< const Char * const >( argv_, argc_ ? argc_ : 1 ) );
		}


		/// Add a group of parameters.
		Parameter_set & operator<<( const Group & gr_ ) 	{
			m_groups.push_back( gr_ );
			return *this;
		}

		/// Add a parameter.
		template< typename ...T >
		Parameter_set & operator()( T && ...t_ )			{
			m_groups.front()( std::forward< T >( t_ )... );
			return *this;
		}

		/// Stream parameters' description to out_.
		template< typename Out >
		void parameters( Out & out_ )			const	{
			for( const auto & group : m_groups )
				group.description( out_ );
		}
		
	private:
		std::vector< Group >					m_groups{};
		std::string								m_version, m_description, m_usage;

		template< typename Container, typename Short2long >
		void parse(
			const Container					  & args_,
			Arguments_type					  & arguments,
			const Short2long				  & all_id
		) const;

		// Make a span from an R-referenced std::vector.
		/* Warning: Unless you know what you do, there is a great risk of a dangling span...	*/
		template< typename T, typename Alloc >
		static std::span< T > constexpr moved_span( std::vector< T, Alloc > && vec_ ) noexcept {
			return { vec_.data(), vec_.size() };
		}
	};


	
	/// Group values together.
	template< Character Char >
	class Parameter_set< Char >::Group {
		using value_type					  = Parameter;
		std::basic_string< Char >				m_description;
		std::vector< value_type >				m_values;

	public:
		Group() : m_description{} {}
		Group( std::basic_string< Char > && description_ ) : m_description{ description_ } {}
		Group( const Group & )				  =	default;
		Group( Group && )					  =	default;
		Group & operator=( const Group & )	  =	default;
		Group & operator=( Group && )		  =	default;

		/// Add a value.
		Group & operator()( value_type && value_ ) 
		{	m_values.push_back( std::move( value_ ) );					return *this;		}

		/// Add a value.
		Group & operator()( const value_type & value_ ) 
		{	m_values.push_back( value_ );								return *this;		}

		/// Construct and add a value.
		template< typename ...T >
		Group & operator()( T && ...t_ )
		{	m_values.emplace_back( std::forward< T >( t_ )... );		return *this;		}
	
		/// Access to the Group's values are through iterators.
		auto begin()		const noexcept	{	return m_values.cbegin();					}
	
		/// Access to the Group's values are through iterators.
		auto end()			const noexcept	{	return m_values.cend();						}

		/// Stream the Group's description and its values to out_.
		template< typename Out >
		Out & description( Out & out_ ) const {
			bool show{ false };
			for( const auto & value : *this )	show = show || value.visible();
			if( show ) {
				if( m_description.size() )	out_ << "\n\033[3m" << m_description << "\033[0m\n";
				for( const auto & value : *this )	value.description( out_ );
			}
			return out_;
		}
	};



	/// Implements a typical parameter. 
	/** The actual values to the parameter are stored in Arguments. Only parameter properties are stored in Parameter.
		A Parameter is one of four types:
		- Flag: Is set with --id and cleared by --no-id. It will not take a value. 
		  If followed by a value, the value will go to unnamed.
		- Zero_or_more_values and One_or_more_values: Can accept multiple values.
		- Scalar: If not set to one of the above, the Parameter will only accept one value (could be the default value).

		A Parameter can be Invisible, it will not be listed by --help.

		A Parameter can have a default value. Flags are always unset by default. 
	
		The consteuctor accepts Is_flag{}, Zero_or_more_values{}, One_or_more_values{}, Invisible{}, and 
		Default_value{ "value" } to set the above properties. 
	**/
	template< Character Char >
	class Parameter_set< Char >::Parameter : public Parameter_type {
		using String						  = std::basic_string< Char >;
		String									m_id, m_description;
		Default_value< Char >					m_default_value;
		Char									m_abbreviation;
		bool									m_visible{ true };
	
	public:

		/// Parameter with an abbreviation.
		template< typename ...Properts >
		Parameter(
			const Char							abbreviation_,
			const String_view					id_,
			const String_view					description_,
			Properts						 && ...properties_
		) :	m_id{ id_ }, 
			m_description{ description_ }, 
			m_abbreviation{ abbreviation_ }
		{
			for( const auto c : id_ ) if( !std::isalnum( c ) && ( c != Char( '-' ) ) && ( c != Char( '_' ) ) )
				throw user_error_message( std20::format( "Parameter '--{}' contains an illegal character '{}'.", id_, c ) );
			if( !std::isgraph( abbreviation_ ) && ( abbreviation_ != '-' ) && ( abbreviation_ != 0 ) )
				throw user_error_message( std20::format( "Abbreviation '-{}' of parameter '--{}' is an illegal character.", abbreviation_, id_ ) );
			m_set( std::forward< Properts >( properties_ )... );
		}
	
		Parameter( const Parameter & )				  =	default;
		Parameter( Parameter && )					  =	default;
		Parameter & operator=( const Parameter & )	  =	default;
		Parameter & operator=( Parameter && )		  =	default;

		/// Parameter without aa abbreviation.
		template< typename ...Properts >
		Parameter(
			const String_view					id_,
			const String_view					description_,
			Properts						 && ...properties_
		) : Parameter( Char{}, id_, description_, std::forward< Properts >( properties_ )... ) {}
	
		/// Get the abbreviation character.
		Char abbreviation()						const noexcept	{	return m_abbreviation;					}
	
		/// Get the name string.
		String_view id()						const noexcept	{	return m_id;							}

		/// Is the parameter visible?
		constexpr bool visible() 				const noexcept	{	return m_visible;						}

		/// Stream the parameter's description to out_.
		template< typename Out >
		Out & description( Out & out_ ) 		const			{
			if( visible() ) {
				std::string						str = std20::format( "--{} ", id() );
				if( abbreviation() != 0 )		str = std20::format( "-{} {}", abbreviation(), str );
				Terminal{}.wrap( out_, 25, std20::format( 
					"  {:23}{} [{}{}]\n", 
						str, 
						m_description, 
						this->meta(),
						m_default_value.empty() 
							? ""
							: std20::format( "='{}'", m_default_value.value() ) 
				) );
			}
			return out_;
		}

		/// Are the parameter's conditions fullfilled with values_?
		template< typename Values >
		void check( const Values & values_ )	const			{
			switch( this->type() ) {
				case scalar().type():
					if( values_.size() > 1 )
						throw user_error_message( std20::format( "Parameter '--{}' is a scalar, but was given multiple values '{}' and '{}'.",
							id(), values_.front(), values_.back() ) );
					[[fallthrough]];
				case one_or_more_values().type():
					if( values_.empty() && m_default_value.empty() )
						throw user_error_message( std20::format( "Parameter '--{}' requires a value, but none was given.", id() ) );
					break;
				case on_flag().type():
				case off_flag().type():
					if( values_.size() != 1 )
						throw user_error_message( std20::format( "Parameter '--{}' is a flag and should not be given values.", id() ) );
					if( !is_flag_on( String_view( values_.front() ) ) && !is_flag_off( String_view( values_.front() ) ) )
						throw user_error_message( std20::format( "Parameter '--{}' is a flag but was given a value ('{}').", id(), values_.front() ) );
					break;
				case zero_or_more_values().type():
					break;
			}
		}

		/// Add the default value to values_, if necessary, and then call check( values_ ). 
		template< typename Values >
		void finalize( Values & values_ )		const			{
			// Add the default value, where needed.
			if( values_.empty() && !m_default_value.empty() )
				values_.push_back( String( m_default_value.value() ) );
			check( values_ );
		}
		
		
		/// Given a name, it removes a possible "no-" prefix.
		static constexpr String_view name_without_turn_off( const String_view name_ ) noexcept 
		{	return name_.starts_with( String_view( "no-" ) ) ? name_.substr( 3 ) : name_;												}
		
		
		/// Given a name, it removes a possible pre- or postfix. 
		/** E.g. "--flag", "flag", "--no-flag" and "no-flag" will all return "flag".		**/
		static constexpr String_view base_name( const String_view name_ ) noexcept 
		{	return name_.starts_with( String_view( "--" ) ) ? base_name( name_.substr( 2 ) ) : name_without_turn_off( name_ );		}

	private:
		void m_set()															noexcept {}

		template< typename ...Properts >
		void m_set( const Parameter_type prop_, Properts && ...properties_ )	noexcept 
		{	dynamic_cast< Parameter_type & >( *this ) = prop_;	m_set( std::forward< Properts >( properties_ )... );	}

		template< typename ...Properts >
		void m_set( Invisible, Properts && ...properties_ )						noexcept
		{	m_visible = false;									m_set( std::forward< Properts >( properties_ )... );	}

		template< typename ...Properts >
		void m_set( Default_value< Char > dv_, Properts && ...properties_ )		noexcept
		{	m_default_value = dv_;								m_set( std::forward< Properts >( properties_ )... );	}
	};


	
	
	/// Output a description.
	template< Character Char >
	auto & operator<<( std::basic_ostream< Char > & out_, const Parameter_set< Char > & parameters_ ) {
		parameters_.parameters( out_ );
		return out_;
	}




	/// Parse the arguments using the 'rules' of the Parameter_set. 
	template< Character Char >
	template< std::size_t N >
	Arguments< Char > Parameter_set< Char >::parse( const std::span< const Char * const, N > argv_ ) const {
		struct All_id {
			All_id() noexcept {}
			
			All_id( const std::vector< Group > & groups_ ) {
				for( const Group & group : groups_ ) for( const Parameter & param_ : group ) {
					const auto in		  = m_longs.insert( std::pair( param_.id(),  param_ ) );
					if( !in.second )
						throw user_error_message( std20::format( "Multiple definitions of parameter '--{}'.", param_.id() ) );
					if( param_.abbreviation() ) {
						const auto in	  = m_abbrevs.insert( std::pair( param_.abbreviation(), param_.id() ) );
						if( !in.second )
							throw user_error_message( std20::format( "Multiple parameters ('--{}' and '--{}') have the same abbreviation '-{}'.", 
												in.first->second, param_.id(), param_.abbreviation() ) );
					}
				}
			}
			
			auto begin()											const noexcept	{	return m_longs.cbegin();						}
			auto end()												const noexcept	{	return m_longs.cend();							}

			// Is there any parameter named id_ (prefixed "--") or with abbreviation id_ (prefixed "-")?
			bool exists( const String_view id_ )					const noexcept	{
				return	( id_.starts_with( String_view( "--" ) ) && ( m_longs.find( String( Parameter::base_name( id_ ) ) ) != m_longs.end() ) )		// long-id exists?
					||	( id_.starts_with( String_view( "-"  ) ) && ( m_abbrevs.find( id_.back() ) != m_abbrevs.end() ) && ( id_.size() == 2 ) );	// abbreviation exists?
			}

			// What is the long id of abbreviation_?
			String_view long_id( const Char abbreviation_ )			const			{
				const auto itr = m_abbrevs.find( abbreviation_ );
				if( itr == m_abbrevs.end() )	throw user_error_message( std20::format( "Unknown parameter abbreviation '-{}'.", abbreviation_ ) );
				return itr->second;
			}

			// Get the parameter.
			const Parameter & parameter( const String_view long_ )	const			{
				const auto itr = m_longs.find( String( Parameter::base_name( long_ ) ) );
				if( itr == m_longs.end() )		throw user_error_message( std20::format( "Unknown parameter '--{}'.", long_ ) );
				return itr->second;
			}

			const Parameter & parameter( const Char abbreviation_ )	const			{	return parameter( long_id( abbreviation_ ) );	}
			
		private:
			std::map< String, Parameter >		m_longs{};
			std::map< Char,   String >			m_abbrevs{};
		};

		Arguments_type							arguments{};

		try {
			const All_id						all_id( m_groups );

			// Fill the arguments container.
			for( auto & item : all_id ) 		arguments.insert( item.second );

			// The actual parsing.
			parse( argv_, arguments, all_id );

			// Check for the special flags, --help and --version.
			if( arguments.flag( "help" ) ) {
				if( m_description.size() )	std::cout << m_description << '\n';
				if( m_usage.size() )		std::cout << "\nUsage:\n" ANSI_BOLD << m_usage << ANSI_RESET"\n\n";
				parameters( std::cout );
				std::cout << '\n';
				exit( EXIT_SUCCESS );
			} else if( arguments.flag( "version" ) ) {
				if( m_version.size() )		std::cout << m_version;
				else						std::cout << argv_.front();
				std::cout << '\n';
				exit( EXIT_SUCCESS );
			}

			// Check that the parse went ok. 
			for( auto & item : all_id )
				arguments.finalize( item.second );

		} catch( const std::exception & error_ ) {
			throw user_error_message( std20::format( "Error: {} Use argument --help for info.", no_control_suffix( error_.what() ) ) );
		}

		return arguments;
	}


	/// Parse the arguments using the 'rules' of the Parameter_set. 
	template< Character Char >
	template< typename Container, typename Short2long >
	void Parameter_set< Char >::parse(
		const Container							  & args_,
		Arguments_type							  & arguments,
		const Short2long						  & all_id
	) const {
		struct {	// The m_id is always either the latest parameter id or anonymous.
			constexpr operator String_view()		const noexcept	{	return m_id;								}
			constexpr void operator=( String_view id_ ) noexcept	{	m_id = id_;									}
			constexpr bool yes()					const noexcept	{	return m_id.data() != id_anonymous.data();	}
			constexpr void clear()					noexcept		{	m_id = id_anonymous;						}
			private: String_view					m_id{ id_anonymous };
		}	arg_id{};

		// Parse all elements of argv_ and set the apropriate argument values in arguments.
		auto itr{ args_.begin() };
		while( ++itr < args_.end() ) {
			const String_view arg				  = *itr;
			if( arg.empty() ) {															// Empty argument (should not happen).
			} else if( arg[ 0 ] == '-' ) {
				if( arg_id.yes() ) {													// Id/flag (from previous argument).
					arguments.set( all_id.parameter( arg_id ), String_view( arg_id ), String_view{} );
					arg_id.clear();
				}
				if( arg.size() == 1 ) {				throw user_error_message( "Empty parameter abbrev. '-'." );
				} else if( arg[ 1 ] == '-' ) {											// New id argument.					
					if( arg.size() == 2 )			throw user_error_message( "Empty parameter '--'." );
					const auto divider			  = arg.find( '=' );
					arg_id						  = arg.substr( 2, divider-2 );
					if( divider < arg.size() ) {										// New id & value, i.e. '--id=value'.
						arguments.set( all_id.parameter( arg_id ), arg_id, arg.substr( divider + 1 ) );
						if( String_view( arg_id ) == m_args_file ) {
							const auto items	  = Parse_json::file( String( arg.substr( divider + 1 ) ), all_id  );
							parse( items, arguments, all_id );
						}
						arg_id.clear();
					}
				} else {																// One (or more) abbreviation(s).
					for( std::size_t j=1; j<arg.size(); ++j ) {
						if( arg[ j ] == '=' ) {											// Value for an abbreviation.
							if( j == 1 )			throw user_error_message( std20::format( "Strangely placed '=' in '{}'.", arg ) );
							arguments.set( all_id.parameter( arg_id ), arg_id, arg.substr( j+1 ) );
							if( String_view( arg_id ) == m_args_file )
								parse( moved_span( Parse_json::file( String( arg.substr( j+1 ) ), all_id ) ), arguments, all_id );
							arg_id.clear();
							j = arg.size();			// to exit the j-loop...
						} else {														// An abbreviation, i.e. '-a' or b in '-ab'
							if( arg_id.yes() )		arguments.set( all_id.parameter( arg_id ), arg_id, "" );	// Flag in previous j iteration.
							arg_id				  = all_id.long_id( arg[ j ] );
						}
					}
				}
			} else {																	// New value argument.
				arguments.set( all_id.parameter( arg_id ), arg_id, arg );
				if( String_view( arg_id ) == m_args_file )
					parse( moved_span( Parse_json::file( String( arg ), all_id ) ), arguments, all_id );
				arg_id.clear();
			}
		}
		if( arg_id.yes() ) {															// Id/flag (from last argument).
			arguments.set( all_id.parameter( arg_id ), arg_id, "" );
			arg_id.clear();
		}
	}





	static_assert(!std::has_virtual_destructor< Parameter_set<> >{} );
	static_assert( std::is_destructible< Parameter_set<> >{} );
	static_assert( std::is_nothrow_destructible< Parameter_set<> >{} );
	static_assert(!std::is_trivially_destructible< Parameter_set<> >{} );

	static_assert( std::is_default_constructible< Parameter_set<> >{} );
	static_assert(!std::is_nothrow_default_constructible< Parameter_set<> >{} );
	static_assert(!std::is_trivially_default_constructible< Parameter_set<> >{} );

	static_assert( std::is_copy_constructible< Parameter_set<> >{} );
	static_assert(!std::is_nothrow_copy_constructible< Parameter_set<> >{} );
	static_assert(!std::is_trivially_copy_constructible< Parameter_set<> >{} );

	static_assert( std::is_copy_assignable< Parameter_set<> >{} );
	static_assert(!std::is_nothrow_copy_assignable< Parameter_set<> >{} );
	static_assert(!std::is_trivially_copy_assignable< Parameter_set<> >{} );

	static_assert( std::is_move_constructible< Parameter_set<> >{} );
	static_assert( std::is_nothrow_move_constructible< Parameter_set<> >{} );
	static_assert(!std::is_trivially_move_constructible< Parameter_set<> >{} );

	static_assert( std::is_move_assignable< Parameter_set<> >{} );
	static_assert( std::is_nothrow_move_assignable< Parameter_set<> >{} );
	static_assert(!std::is_trivially_move_assignable< Parameter_set<> >{} );

	static_assert( std::is_swappable< Parameter_set<> >{} );
	static_assert( std::is_nothrow_swappable< Parameter_set<> >{} );



	static_assert(!std::has_virtual_destructor< Parameter_set< char >::Parameter >{} );
	static_assert( std::is_nothrow_destructible< Parameter_set< char >::Parameter >{} );
	static_assert(!std::is_trivially_destructible< Parameter_set< char >::Parameter >{} );

	static_assert(!std::is_default_constructible< Parameter_set< char >::Parameter >{} );
	static_assert(!std::is_nothrow_default_constructible< Parameter_set< char >::Parameter >{} );
	static_assert(!std::is_trivially_default_constructible< Parameter_set< char >::Parameter >{} );

	static_assert( std::is_copy_constructible< Parameter_set< char >::Parameter >{} );
	static_assert(!std::is_nothrow_copy_constructible< Parameter_set< char >::Parameter >{} );
	static_assert(!std::is_trivially_copy_constructible< Parameter_set< char >::Parameter >{} );

	static_assert( std::is_copy_assignable< Parameter_set< char >::Parameter >{} );
	static_assert(!std::is_nothrow_copy_assignable< Parameter_set< char >::Parameter >{} );
	static_assert(!std::is_trivially_copy_assignable< Parameter_set< char >::Parameter >{} );

	static_assert( std::is_move_constructible< Parameter_set< char >::Parameter >{} );
	static_assert( std::is_nothrow_move_constructible< Parameter_set< char >::Parameter >{} );
	static_assert(!std::is_trivially_move_constructible< Parameter_set< char >::Parameter >{} );

	static_assert( std::is_move_assignable< Parameter_set< char >::Parameter >{} );
	static_assert( std::is_nothrow_move_assignable< Parameter_set< char >::Parameter >{} );
	static_assert(!std::is_trivially_move_assignable< Parameter_set< char >::Parameter >{} );

	static_assert( std::is_swappable< Parameter_set< char >::Parameter >{} );
	static_assert( std::is_nothrow_swappable< Parameter_set< char >::Parameter >{} );



	static_assert(!std::has_virtual_destructor< Parameter_set< char >::Group >{} );
	static_assert( std::is_nothrow_destructible< Parameter_set< char >::Group >{} );
	static_assert(!std::is_trivially_destructible< Parameter_set< char >::Group >{} );

	static_assert( std::is_default_constructible< Parameter_set< char >::Group >{} );
	static_assert(!std::is_nothrow_default_constructible< Parameter_set< char >::Group >{} );
	static_assert(!std::is_trivially_default_constructible< Parameter_set< char >::Group >{} );

	static_assert( std::is_copy_constructible< Parameter_set< char >::Group >{} );
	static_assert(!std::is_nothrow_copy_constructible< Parameter_set< char >::Group >{} );
	static_assert(!std::is_trivially_copy_constructible< Parameter_set< char >::Group >{} );

	static_assert( std::is_copy_assignable< Parameter_set< char >::Group >{} );
	static_assert(!std::is_nothrow_copy_assignable< Parameter_set< char >::Group >{} );
	static_assert(!std::is_trivially_copy_assignable< Parameter_set< char >::Group >{} );

	static_assert( std::is_move_constructible< Parameter_set< char >::Group >{} );
	static_assert( std::is_nothrow_move_constructible< Parameter_set< char >::Group >{} );
	static_assert(!std::is_trivially_move_constructible< Parameter_set< char >::Group >{} );

	static_assert( std::is_move_assignable< Parameter_set< char >::Group >{} );
	static_assert( std::is_nothrow_move_assignable< Parameter_set< char >::Group >{} );
	static_assert(!std::is_trivially_move_assignable< Parameter_set< char >::Group >{} );

	static_assert( std::is_swappable< Parameter_set< char >::Group >{} );
	static_assert( std::is_nothrow_swappable< Parameter_set< char >::Group >{} );

}	// namespace pax::cmd_args

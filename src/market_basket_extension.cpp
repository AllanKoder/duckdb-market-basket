#define DUCKDB_EXTENSION_MAIN

#include "market_basket_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/function/scalar_function.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

// OpenSSL linked through vcpkg
#include <openssl/opensslv.h>

namespace duckdb {

inline void MarketBasketScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &name_vector = args.data[0];
	UnaryExecutor::Execute<string_t, string_t>(name_vector, result, args.size(), [&](string_t name) {
		return StringVector::AddString(result, "...........🦆 " + name.GetString());
	});
}

inline void MarketBasketOpenSSLVersionScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &name_vector = args.data[0];
	UnaryExecutor::Execute<string_t, string_t>(name_vector, result, args.size(), [&](string_t name) {
		return StringVector::AddString(result, "MarketBasket " + name.GetString() + ", my linked OpenSSL version is " +
		                                           OPENSSL_VERSION_TEXT);
	});
}

static void LoadInternal(ExtensionLoader &loader) {
	// Register a scalar function
	auto market_basket_scalar_function =
	    ScalarFunction("market_basket", {LogicalType::VARCHAR}, LogicalType::VARCHAR, MarketBasketScalarFun);

	loader.RegisterFunction(market_basket_scalar_function);

	// Register another scalar function
	auto market_basket_openssl_version_scalar_function =
	    ScalarFunction("market_basket_openssl_version", {LogicalType::VARCHAR}, LogicalType::VARCHAR,
	                   MarketBasketOpenSSLVersionScalarFun);
	loader.RegisterFunction(market_basket_openssl_version_scalar_function);
}

void MarketBasketExtension::Load(ExtensionLoader &loader) {
	LoadInternal(loader);
}
std::string MarketBasketExtension::Name() {
	return "market_basket";
}

std::string MarketBasketExtension::Version() const {
#ifdef EXT_VERSION_MARKET_BASKET
	return EXT_VERSION_MARKET_BASKET;
#else
	return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_CPP_EXTENSION_ENTRY(market_basket, loader) {
	duckdb::LoadInternal(loader);
}
}

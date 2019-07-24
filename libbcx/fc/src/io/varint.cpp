#include <fc/io/varint.hpp>
#include <fc/variant.hpp>

namespace fc
{
void to_variant( const unsigned_int& var, variant& vo, uint32_t max_depth )  { vo = var.value; }
void from_variant( const variant& var, unsigned_int& vo, uint32_t max_depth )  { vo.value = var.as_uint64(); }
}

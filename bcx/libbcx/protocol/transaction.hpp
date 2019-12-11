#pragma once

#include "./types.hpp"
#include "./base.hpp"
#include "./operations/operations.hpp"
#include "../fc/include/fc/reflect/reflect.hpp"

namespace bcx {

namespace protocol {

struct transaction {
    /**
    * Least significant 16 bits from the reference block number. If @ref relative_expiration is zero, this field
    * must be zero as well.
    */
    uint16_t           ref_block_num    = 0;
    /**
    * The first non-block-number 32-bits of the reference block ID. Recall that block IDs have 32 bits of block
    * number followed by the actual block hash, so this field should be set using the second 32 bits in the
    * @ref block_id_type
    */
    uint32_t           ref_block_prefix = 0;

    /**
    * This field specifies the absolute expiration for this transaction.
    */
    fc::time_point_sec expiration;

    std::vector<operation>  operations;
    extensions_type    extensions;

    /// Calculate the digest used for signature validation
    digest_type         sig_digest( const chain_id_type& chain_id )const;

    void set_expiration( fc::time_point_sec expiration_time );
    void set_reference_block( const block_id_type& reference_block );

    /// visit all operations
    template<typename Visitor>
    std::vector<typename Visitor::result_type> visit( Visitor&& visitor ) {
        std::vector<typename Visitor::result_type> results;
        for( auto& op : operations )
            results.push_back(op.visit( std::forward<Visitor>( visitor ) ));
        return results;
    }

    template<typename Visitor>
    std::vector<typename Visitor::result_type> visit( Visitor&& visitor )const {
        std::vector<typename Visitor::result_type> results;
        for( auto& op : operations )
            results.push_back(op.visit( std::forward<Visitor>( visitor ) ));
        return results;
    }

};

/**
*  @brief adds a signature to a transaction
*/
struct signed_transaction : public transaction {
    signed_transaction( const transaction& trx = transaction() ): transaction(trx){}

    /** signs and appends to signatures */
    const signature_type& sign( const private_key_type& key, const chain_id_type& chain_id );

    /** returns signature but does not append */
    signature_type sign( const private_key_type& key, const chain_id_type& chain_id )const;

    /** Signatures */
    std::vector<signature_type> signatures;
};


} // namespace protocol

} // namespace bcx


FC_REFLECT( bcx::protocol::transaction,
           (ref_block_num)(ref_block_prefix)(expiration)(operations)(extensions)
           )

FC_REFLECT_DERIVED( bcx::protocol::signed_transaction, (bcx::protocol::transaction),
                   (signatures)
                   )



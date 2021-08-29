#ifndef P2P_FILE_SYNC_UTILS_STATUS_H
#define P2P_FILE_SYNC_UTILS_STATUS_H

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/ecdh.h>
#include <openssl/ecdsa.h>

void generate_key_pair(string & pub_key, string & priv_key){
    EC_KEY *ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    assert(1==EC_KEY_generate_key(ec_key));
    assert(1==EC_KEY_check_key(ec_key));

    BIO * bio = BIO_new_fp(stdout,0);
    //assert(1==EC_KEY_print(bio, ec_key, 0));
    BIO_free(bio);

    {
        FILE * f = fopen(pub_key.c_str(),"w");
        PEM_write_EC_PUBKEY(f, ec_key);
        //PEM_write_bio_EC_PUBKEY(bio, ec_key);
        fclose(f);
    }

    {
        FILE * f = fopen(priv_key.c_str(),"w");
        PEM_write_ECPrivateKey(f,ec_key, NULL,NULL,0,NULL,NULL);
        //PEM_write_bio_ECPrivateKey(bio,ec_key, NULL,NULL,0,NULL,NULL);
        fclose(f);
    }

    EC_KEY_free(ec_key);
}

#endif
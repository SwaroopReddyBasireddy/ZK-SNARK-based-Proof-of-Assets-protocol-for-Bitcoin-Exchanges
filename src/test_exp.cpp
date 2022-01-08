#include <stdlib.h>
#include <iostream>

#include "libff/algebra/fields/field_utils.hpp"
#include "libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp"
#include "libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp"
#include "libsnark/gadgetlib1/pb_variable.hpp"

#include "util.hpp"

using namespace libsnark;
using namespace std;

int main()
{
  typedef libff::Fr<default_r1cs_gg_ppzksnark_pp> FieldT;

  // Initialize the curve parameters

  default_r1cs_gg_ppzksnark_pp::init_public_params();
  
  // Create protoboard

  protoboard<FieldT> pb;

  // Define variables

  pb_variable<FieldT> x1;
  pb_variable<FieldT> x2;
  pb_variable<FieldT> x3;
  pb_variable<FieldT> x4;
 // pb_variable<FieldT> sym_1;
  pb_variable<FieldT> y;
 // pb_variable<FieldT> sym_2;
  pb_variable<FieldT> out;

  // Allocate variables to protoboard
  // The strings (like "x") are only for debugging purposes
  
  out.allocate(pb, "out");
  x1.allocate(pb, "x1");
  x2.allocate(pb, "x2");
  x3.allocate(pb, "x3");
  x4.allocate(pb, "x4");
 // sym_1.allocate(pb, "sym_1");
  y.allocate(pb, "y");
 // sym_2.allocate(pb, "sym_2");

  // This sets up the protoboard variables
  // so that the first one (out) represents the public
  // input and the rest is private input
  pb.set_input_sizes(1);

  // Add R1CS constraints to protoboard

  // (x1+x2)*x3 = y
  pb.add_r1cs_constraint(r1cs_constraint<FieldT>(x1+x2, x3, y));

  
  // y*x4 = ~out
  pb.add_r1cs_constraint(r1cs_constraint<FieldT>(y, x4, out));
  
  // Add witness values

  pb.val(x1) = 1;
  pb.val(x2) = 2;
  pb.val(x3) = 1;
  pb.val(x4) = 5;
  pb.val(out) = 15;
  pb.val(y) = 3;
  

  const r1cs_constraint_system<FieldT> constraint_system = pb.get_constraint_system();
  const r1cs_gg_ppzksnark_keypair<default_r1cs_gg_ppzksnark_pp> keypair = r1cs_gg_ppzksnark_generator<default_r1cs_gg_ppzksnark_pp>(constraint_system);
  
  const r1cs_gg_ppzksnark_proof<default_r1cs_gg_ppzksnark_pp> proof = r1cs_gg_ppzksnark_prover<default_r1cs_gg_ppzksnark_pp>(keypair.pk, pb.primary_input(), pb.auxiliary_input());

  
  bool verified = r1cs_gg_ppzksnark_verifier_strong_IC<default_r1cs_gg_ppzksnark_pp>(keypair.vk, pb.primary_input(), proof);

  cout << sizeof(proof) << endl;
  
  cout << "Number of R1CS constraints: " << constraint_system.num_constraints() << endl;
  cout << "Primary (public) input: " << pb.primary_input() << endl;
  cout << "Auxiliary (private) input: " << pb.auxiliary_input() << endl;
  cout << "Verification status: " << verified << endl;

  const r1cs_gg_ppzksnark_verification_key<default_r1cs_gg_ppzksnark_pp> vk = keypair.vk;

 // print_vk_to_file<default_r1cs_gg_ppzksnark_pp>(vk, "../build/vk_data");
 // print_proof_to_file<default_r1cs_gg_ppzksnark_pp>(proof, "../build/proof_data");
 
 

  return 0;
}
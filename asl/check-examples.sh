#!/bin/bash


 echo ""
 echo "BEGIN examples-full/typecheck"
 for f in ../examples/jp_chkt_*.asl; do
     echo $(basename $f)
     ./asl $f | egrep ^L > tmp.err
     diff tmp.err ${f/asl/err}
     rm -f tmp.err
 done
 echo "END   examples-full/typecheck"


 echo ""
 echo "BEGIN examples-full/codegen"
 for f in ../examples/jp_genc_*.asl; do
     echo $(basename "$f")
     ./asl "$f" > tmp.t
     diff tmp.t "${f/asl/t}"
     rm -f tmp.t
 done
 echo "END   examples-full/codegen"


 echo ""
 echo "BEGIN examples-full/execution"
 for f in ../examples/jp_genc_*.asl; do
     echo $(basename "$f")
     ./asl "$f" > tmp.t
     ../tvm/tvm tmp.t < "${f/asl/in}" > tmp.out
     diff tmp.out "${f/asl/out}"
     rm -f tmp.t tmp.out
 done
 echo "END   examples-full/execution"

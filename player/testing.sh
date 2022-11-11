# Make dealer output file
rm -rf dealer.out ; touch dealer.out
# Making everything
cd .. ; make ; cd player
# Testing with no arguments
./player
# Testing with bad port
./player whatevername whateverip 0
# Testing with dealer
./dealer 10 8092 > dealer.out 2>&1 & 
IPADDR=$(curl ifconfig.me)
./player Name $IPADDR 8092


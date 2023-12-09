rm mmu
make mmu
./mmu input0.txt -F > out1F
./mmu input0.txt -B > out1B
./mmu input0.txt -W > out1W
./mmu_ref input0.txt -F > out2F
./mmu_ref input0.txt -B > out2B
./mmu_ref input0.txt -W > out2W
diff out1F out2F
diff out1B out2B
diff out1W out2W

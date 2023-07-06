all:
	cd ./glucose/simp; make; cd -; mkdir -p bin && cp glucose/simp/glucose bin/glucose

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
export ZES_ENABLE_SYSMAN=1

./testlibapmidg

python clr_apmidg.py

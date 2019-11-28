# SkipList
SkipList implemented in c++11
# Benchmark
Tested on my i5-7500 3.40GHz, see [test](test.cc).

```
10000 elements random insert,timespan=6
10000 elements random query by index,timespan=2
10000 elements random query by data,timespan=2
10000 elements random query index of data,timespan=2
10000 elements random delete,timespan=9
------------------------------------------------------------------
100000 elements random insert,timespan=102
100000 elements random query by index,timespan=60
100000 elements random query by data,timespan=70
100000 elements random query index of data,timespan=70
100000 elements random delete,timespan=100
------------------------------------------------------------------
1000000 elements random insert,timespan=1847
1000000 elements random query by index,timespan=1354
1000000 elements random query by data,timespan=1622
1000000 elements random query index of data,timespan=1652
1000000 elements random delete,timespan=1880
------------------------------------------------------------------
```

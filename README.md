# pollop
c++ distributed system from scratch

##  Raft 
Figure 8 in Paper:
The problem is: with current setting, raft could rewrite the logs that have been put to majority servers, which does not make sense.
So new leader in new term shouldn't replicate the logs from past term. Rather, it should only replicate the logs from current term.
Then: "once an entry
from the current term has been committed in this way,
then all prior entries are committed indirectly because
of the Log Matching Property. "

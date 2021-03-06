#ifndef CLICK_XARPTABLE_HH
#define CLICK_XARPTABLE_HH
#include <click/element.hh>
#include <click/etheraddress.hh>
#include <click/hashcontainer.hh>
#include <click/hashallocator.hh>
#include <click/sync.hh>
#include <click/timer.hh>
#include <click/list.hh>
#include <click/xid.hh>
CLICK_DECLS

/*
=c

XARPTable(I<keywords>)

=s arp

stores IP-to-Ethernet mappings

=d

The XARPTable element stores IP-to-Ethernet mappings, such as are useful for
the XARP protocol.  XARPTable is an information element, with no inputs or
outputs.  XARPQuerier normally encapsulates access to an XARPTable element.  A
separate XARPTable is useful if several XARPQuerier elements should share a
table.

Keyword arguments are:

=over 8

=item CAPACITY

Unsigned integer.  The maximum number of saved IP packets the XARPTable will
hold at a time.  Default is 2048; zero means unlimited.

=item ENTRY_CAPACITY

Unsigned integer.  The maximum number of XARP entries the XARPTable will hold at
a time.  Default is zero, which means unlimited.

=item TIMEOUT

Time value.  The amount of time after which an XARP entry will expire.  Default
is 5 minutes.  Zero means XARP entries never expire.

=h table r

Return a table of the XARP entries.  The returned string has four
space-separated columns: an IP address, whether the entry is valid (1 means
valid, 0 means not), the corresponding Ethernet address, and finally, the
amount of time since the entry was last updated.

=h drops r

Return the number of packets dropped because of timeouts or capacity limits.

=h insert w

Add an entry to the table.  The format should be "IP ETH".

=h delete w

Delete an entry from the table.  The string should consist of an IP address.

=h clear w

Clear the table, deleting all entries.

=h count r

Return the number of entries in the table.

=h length r

Return the number of packets stored in the table.

=a

XARPQuerier
*/

class XARPTable : public Element { public:

    XARPTable();
    ~XARPTable();

    const char *class_name() const		{ return "XARPTable"; }

    int configure(Vector<String> &, ErrorHandler *);
    bool can_live_reconfigure() const		{ return true; }
    void take_state(Element *, ErrorHandler *);
    void add_handlers();
    void cleanup(CleanupStage);

    int lookup(XID xid, EtherAddress *eth, uint32_t poll_timeout_j);
    EtherAddress lookup(XID xid);
    XID reverse_lookup(const EtherAddress &eth);
    int insert(XID xid, const EtherAddress &en, Packet **head = 0);
    int append_query(XID xid, Packet *p);
    void clear();

    uint32_t capacity() const {
	return _packet_capacity;
    }
    void set_capacity(uint32_t capacity) {
	_packet_capacity = capacity;
    }
    uint32_t entry_capacity() const {
	return _entry_capacity;
    }
    void set_entry_capacity(uint32_t entry_capacity) {
	_entry_capacity = entry_capacity;
    }
    Timestamp timeout() const {
	return Timestamp::make_jiffies((click_jiffies_t) _timeout_j);
    }
    void set_timeout(const Timestamp &timeout) {
	if ((uint32_t) timeout.sec() >= (uint32_t) 0xFFFFFFFFU / CLICK_HZ)
	    _timeout_j = 0;
	else
	    _timeout_j = timeout.jiffies();
    }

    uint32_t drops() const {
	return _drops;
    }
    uint32_t count() const {
	return _entry_count;
    }
    uint32_t length() const {
	return _packet_count;
    }

    void run_timer(Timer *);

    enum {
	h_table, h_insert, h_delete, h_clear
    };
    static String read_handler(Element *e, void *user_data);
    static int write_handler(const String &str, Element *e, void *user_data, ErrorHandler *errh);

    struct XARPEntry {		
	XID _xid;
	XARPEntry *_hashnext;
	EtherAddress _eth;
	bool _known;
	click_jiffies_t _live_at_j;
	click_jiffies_t _polled_at_j;
	Packet *_head;
	Packet *_tail;
	List_member<XARPEntry> _age_link;
	typedef XID key_type;
	typedef XID key_const_reference;
	key_const_reference hashkey() const {
	    return _xid;
	}
	bool expired(click_jiffies_t now, uint32_t timeout_j) const {
	    return click_jiffies_less(_live_at_j + timeout_j, now)
		&& timeout_j;
	}
	bool known(click_jiffies_t now, uint32_t timeout_j) const {
	    return _known && !expired(now, timeout_j);
	}
	XARPEntry(XID xid)
	    : _xid(xid), _hashnext(), _eth(EtherAddress::make_broadcast()),
	      _known(false), _head(), _tail() {
	}
    };

  private:

    ReadWriteLock _lock;

    typedef HashContainer<XARPEntry> Table;
    Table _table;
    typedef List<XARPEntry, &XARPEntry::_age_link> AgeList;
    AgeList _age;
    atomic_uint32_t _entry_count;
    atomic_uint32_t _packet_count;
    uint32_t _entry_capacity;
    uint32_t _packet_capacity;
    uint32_t _timeout_j;
    atomic_uint32_t _drops;
    SizedHashAllocator<sizeof(XARPEntry)> _alloc;
    Timer _expire_timer;

    XARPEntry *ensure(XID xid, click_jiffies_t now);
    void slim(click_jiffies_t now);

};

inline int
XARPTable::lookup(XID xid, EtherAddress *eth, uint32_t poll_timeout_j)
{
    _lock.acquire_read();
    int r = -1;
    if (Table::iterator it = _table.find(xid)) {
	click_jiffies_t now = click_jiffies();
	if (it->known(now, _timeout_j)) {
	    *eth = it->_eth;
	    if (poll_timeout_j
		&& !click_jiffies_less(now, it->_live_at_j + poll_timeout_j)
		&& !click_jiffies_less(now, it->_polled_at_j + (CLICK_HZ / 10))) {
		it->_polled_at_j = now;
		r = 1;
	    } else
		r = 0;
	}
    }
    _lock.release_read();
    return r;
}

inline EtherAddress
XARPTable::lookup(XID xid)
{
    EtherAddress eth;
    if (lookup(xid, &eth, 0) >= 0)
	return eth;
    else
	return EtherAddress::make_broadcast();
}

CLICK_ENDDECLS
#endif

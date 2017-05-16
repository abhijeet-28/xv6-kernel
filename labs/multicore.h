#pragma once
#include<atomic>
//
// INVARIANT: w_deleted_count <= w_deleting_count <= w_cached_read_count <= shared_read_count <= r_reading_count <= r_cached_write_count <= shared_write_count <= w_writing_count <= w_deleted_count + MAX_SIZE
//
// INVARIANT:      w_writing_count      - w_deleted_count     <= MAX_SIZE
// =========>      w_writing_count      - w_cached_read_count <= MAX_SIZE
// =========>      shared_write_count   - w_cached_read_count <= MAX_SIZE
// =========>      shared_write_count   - shared_read_count   <= MAX_SIZE
//
//
// INVARIANT: 0 <= r_cached_write_count - r_reading_count
// =========> 0 <= r_cached_write_count - shared_read_count
// =========> 0 <= shared_write_count   - shared_read_count
//
//
// THEOREM: =========> 0 <= shared_write_count   - shared_read_count   <= MAX_SIZE
//





//
//
// Channel/Queue:
//
// Shared between Producer and Consumer
//
struct channel_t{
  public:


    std::atomic<size_t> actual_read_count,actual_write_count;

    //insert your code here

  public:

    //
    // Intialize
    //
    channel_t(){
      actual_read_count=0;
      actual_write_count=0;
      // insert your code here

    }
};


//
// Producer's (not shared)
//
struct writeport_t{
public:
    //insert your code here
    size_t maxm;
    size_t write_count,delete_count,cached_read_count,deleting_count;

public:

  //
  // Intialize
  //
  writeport_t(size_t tsize)
  {
    maxm=tsize;
    write_count=0;
    delete_count=0;
    deleting_count=0;
    cached_read_count=0;
    //insert code here
  }

public:

  //
  // no of entries available to write
  //
  // helper function for write_canreserve
  //
  size_t write_reservesize(){

    // insert your code here
    return (maxm-(write_count-delete_count));

  
  }

  //
  // Can write 'n' entries?
  //
  bool write_canreserve(size_t n){

    // insert your code here
    if(write_reservesize()>=n)
      return true;
    else
    return false;
  }

  //
  // Reserve 'n' entries for write
  //
  size_t write_reserve(size_t n){
    // insert your code here
    write_count+=n;
    return (write_count-n);

    
  }

  //
  // Commit
  //
  // Read/Write shared memory data structure
  //
  void write_release(channel_t& ch){

    // insert your code here
    ch.actual_write_count=write_count;

  }




public:

  //
  //
  // Read/Write shared memory data structure
  //
  void read_acquire(channel_t& ch){

    //insert your code here
    cached_read_count=ch.actual_read_count;


  }




  //
  // No of entires available to delete
  //
  size_t delete_reservesize(){

    //insert your code here
    return cached_read_count-delete_count;
    
  }

  //
  // Can delete 'n' entires?
  //
  bool delete_canreserve(size_t n){
    if(delete_reservesize()>=n)
      return true;
    //insert your code here
    else
    return false;
  }

  //
  // Reserve 'n' entires for deletion
  //
  size_t delete_reserve(size_t n){
    //insert your code here
    deleting_count=delete_count+n;
    return delete_count;

    
  }


  //
  // Update the state, if any.
  //
  void delete_release(){
    //insert your code here
    delete_count=deleting_count;

  }


};


//
// Consumer's (not shared)
//
//
struct readport_t{
public:

  //insert your code here

  size_t reading_count;
  size_t cached_write_count;
  size_t maxim;

public:
  //
  // Initialize
  //
  readport_t(size_t tsize)
  {
    maxim=tsize;
    reading_count=0;
    cached_write_count=0;
    //insert your code here


  }
  public:

  //
  // Read/Write shared memory data structure
  //
  void write_acquire(channel_t& ch){

    //insert your code here
    cached_write_count=ch.actual_write_count;


  }

  //
  // no of entries available to read
  //
  size_t read_reservesize(){

    //insert your code here
    return cached_write_count-reading_count;


    
  }

  //
  // Can Read 'n' entires?
  //
  bool read_canreserve(size_t n){

    //insert your code here
    if(read_reservesize()>=n)
      return true;
    else
    return false;
  }

  //
  // Reserve 'n' entires to be read
  //
  size_t read_reserve(size_t n){

    //insert your code here
    reading_count+=n;
    return reading_count-n;

    
  }

  //
  // Read/write shared memory data structure
  //
  void read_release(channel_t& ch){

    //insert your code here
    ch.actual_read_count=reading_count;

  }

};



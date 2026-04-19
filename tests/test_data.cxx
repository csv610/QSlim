#include <stdmix.h>
#include <MxDynBlock.h>
#include <MxHeap.h>
#include <cassert>
#include <iostream>

void test_dynblock() {
    MxDynBlock<int> block(4);
    assert(block.length() == 0);
    
    block.add(10);
    block.add(20);
    assert(block.length() == 2);
    assert(block(0) == 10);
    assert(block(1) == 20);
    
    block.drop();
    assert(block.length() == 1);
    
    std::cout << "MxDynBlock tests passed." << std::endl;
}

class MyItem : public MxHeapable {
public:
    int id;
    MyItem(int i, float k) : id(i) { heap_key(k); }
};

void test_heap() {
    MxHeap heap(4);
    MyItem i1(1, 10.0f);
    MyItem i2(2, 5.0f);
    MyItem i3(3, 15.0f);
    
    heap.insert(&i1);
    heap.insert(&i2);
    heap.insert(&i3);
    
    assert(heap.size() == 3);
    
    // MxHeap is a MAX-heap. It returns the largest key first.
    // (QSlim uses it as a min-heap by negating the errors).
    MxHeapable *top = heap.extract();
    assert(((MyItem*)top)->id == 3); // 15.0f is largest
    
    top = heap.extract();
    assert(((MyItem*)top)->id == 1); // 10.0f
    
    top = heap.extract();
    assert(((MyItem*)top)->id == 2); // 5.0f
    
    std::cout << "MxHeap tests passed." << std::endl;
}

int main() {
    test_dynblock();
    test_heap();
    return 0;
}

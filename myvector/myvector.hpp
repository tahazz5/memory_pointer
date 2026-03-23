#pragma once
#include<iostream>
#include<utility>
template<typename T>
class myvector{
    private:
        T* data;
        size_t size_;
        size_t capacity_;
    public:
        myvector():data(nullptr),size_(0),capacity_(0){}
        myvector(size_t size):data(new T[2*size]),size_(size),capacity_(2*size){

        }
        ~myvector(){
            delete[] data ;
        }
        myvector(const myvector &other):data(new T[other.size_]),size_(other.size_),capacity_(other.capacity_){
            for(int i=0;i<size_;i++){
                data[i]=other.data[i];
            }
        }
        myvector(myvector &&other)noexcept:data(other.data),size_(other.size_),capacity_(other.capacity_){
            other.data=nullptr ;
            other.size_=0,other.capacity_=0;
        }
        template<typename... Args>
        void emplace_back(Args&&... args){
            push_back(T(std::forward<Args>(args)...));
        }
        myvector& operator=(const myvector &other){
            if(this!=&other){
                delete[]data ;
                data= new T[other.size_];
                for(int i=0;i<size_;i++){
                    data[i]=other.data[i];
                }
                size_=other.size_ ;
                capacity_=other.capacity_;

            }
            return *this;
        }
        myvector& operator=(myvector &&other){
            if(this!=&other){
                delete[] data;
                data= other.data;
                other.data=nullptr ;
                size_=other.size_ ;
                capacity_=other.capacity_;
                other.size_=0;
                other.capacity_=0;


            }
            return *this;
        }
        void push_back(const T& value){
            if(capacity_>size_){
                data[size_]=value;
                size_++;
            }
            else{
                size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
                T* tmp = new T[new_capacity];
                capacity_=new_capacity;
                for(int i =0 ;i<size_; i++){
                    tmp[i]=std::move(data[i]);
                }
                tmp[size_]=value;
                size_++;
                delete[] data;
                data=tmp;

            }

        }
        void push_back(T&& value){
            if(capacity_>size_){
                data[size_]=std::move(value);
                size_++;
            }
            else{
                size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
                T* tmp = new T[new_capacity];
                capacity_=new_capacity;
                for(int i =0 ;i<size_; i++){
                    tmp[i]=std::move(data[i]);
                }
                tmp[size_]=std::move(value);
                size_++;
                delete[] data;
                data=tmp;

            }

        }
        

        size_t size() const{
            return size_;
        }
        size_t capacity() const{
            return capacity_;
        }

        T& operator[](size_t i){
            return data[i];
        }
        T*begin(){ return data_; }
        T*end(){ return data_ + size_; }


};
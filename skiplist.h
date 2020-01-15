
#pragma once

#include<iostream>
#include<time.h>
#include<math.h>
using namespace std;

#define TAIL_INFINITY_KEY 0xffffff

template<class KeyType,class ValueType>
struct Element{
    KeyType key;
    ValueType value;
    Element(){}
    Element(KeyType key,const ValueType& value){
        this->key=key;
        this->value=value;
    }
};

template<class KeyType,class ValueType>
struct skipNode{
    Element<KeyType,ValueType> element;
    skipNode**next;
    skipNode(const Element<KeyType,ValueType>&theElement,int size){
        this->element=theElement;
        this->next=new skipNode<KeyType,ValueType>*[size];
    }
};

template<class KeyType,class ValueType>
class skiplist {
private:
    // 概率
    float m_prob;
    KeyType m_tailKey;
    // 最大索引层数
    int m_maxLevel;
    // 当前某个节点的最大索引层数 1 <= m_curMaxLevel <= m_maxLevel
    int m_curMaxLevel;
    int m_size;
    // 头结点
    skipNode<KeyType,ValueType>* m_headNode;
    // 尾节点/哨兵节点
    skipNode<KeyType,ValueType>* m_tailNode;
    // 用于保存前驱节点
    skipNode<KeyType,ValueType>** m_forwardNodes;
public:
    skiplist(float prob=0.5f,KeyType tailLargeKey=TAIL_INFINITY_KEY,int max_level=10,int number_node=-1);
    ~skiplist();
    void insert(const Element<KeyType,ValueType>&element);
    bool erase(const KeyType&key);
    Element<KeyType,ValueType>* find(const KeyType&key);
    skipNode<KeyType,ValueType>* search(const KeyType&key);
    int size(){return m_size;}
    void output();
    // 简单随机生成索引节点层数 1<=level<=m_maxLevel
	// int random_level(){
	// 	int level = rand() % m_maxLevel + 1;
	// 	return level;
	// }
    int random_level(){
        int level= 1;
        while (((double)(rand()%101)/101) < (m_prob) && level < m_maxLevel) {
            level++;
        }
        return level;
    }
    int MaxLevel(int numberOfnode,float prob){
        return (int)(ceil(logf((float)numberOfnode)/logf((float)1/prob)));
    }
};


template<class KeyType,class ValueType>
skiplist<KeyType,ValueType>::skiplist(float prob,KeyType tailLargeKey,int max_level,int numberOfnode){
    
    srand(time(nullptr));
    m_prob=prob;
    m_size=0;

    // 计算最大链表层数
    if(numberOfnode!=-1)
        m_maxLevel=MaxLevel(numberOfnode,prob);
    else
        m_maxLevel=max_level;

    m_curMaxLevel=0;

    // 初始化尾节点
    m_tailKey=tailLargeKey;
    Element<KeyType,ValueType>tailPair{m_tailKey,0};
    m_tailNode=new skipNode<KeyType,ValueType>(tailPair,0);

    // 初始化头结点
    m_headNode=new skipNode<KeyType,ValueType>(tailPair,m_maxLevel);
    for (int i = 0; i < m_maxLevel; i++){
        m_headNode->next[i]=m_tailNode;
    }
    m_forwardNodes=new skipNode<KeyType,ValueType>*[m_maxLevel];
}
template<class KeyType,class ValueType>
skiplist<KeyType,ValueType>::~skiplist(){
    while (m_headNode!=m_tailNode){
        auto x=m_headNode->next[0];
        delete m_headNode;
        m_headNode=x;
    }
    delete m_tailNode;
    delete []m_forwardNodes;
}

template<class KeyType,class ValueType>
void skiplist<KeyType,ValueType>::insert(const Element<KeyType,ValueType>&element){
    // 随机生成索引节点层数 1<=level<=m_maxLevel
    int level=random_level();
    if(level > this->m_curMaxLevel){
        this->m_curMaxLevel=level;
    }
    skipNode<KeyType,ValueType>*pNode=search(element.key);
    // 已经存在
    if(pNode->element.key==element.key){
        pNode->element.value=element.value;
        return;
    }
    // 此时已经保存了合适的前驱节点m_forwardNodes
    skipNode<KeyType,ValueType>*pNewNode = new skipNode<KeyType,ValueType>(element,level);
    
    // 建立索引节点
    for (int i = level-1; i >=0; --i){
        pNewNode->next[i]=m_forwardNodes[i]->next[i];
        m_forwardNodes[i]->next[i]=pNewNode;
    }
    m_size++;
}

/*  搜索并把遇到的最后一个节点保存下来 */
template<class KeyType,class ValueType>
skipNode<KeyType,ValueType>* skiplist<KeyType,ValueType>::search(const KeyType&key){
    skipNode<KeyType,ValueType>*forwardNode=m_headNode;
    // 外层循环: 不断的指向下一层
    for (int i = m_curMaxLevel-1; i >= 0; --i){
        // 内层循环: 指向当前层链表的下一个节点
        while (forwardNode->next[i]!=m_tailNode&&forwardNode->next[i]->element.key<key){
            forwardNode=forwardNode->next[i];
        }
        // 保存前驱节点指针
        this->m_forwardNodes[i]=forwardNode;
    }
    // 最终回到第0层
    return forwardNode->next[0];
}

template<class KeyType,class ValueType>
Element<KeyType,ValueType>* skiplist<KeyType,ValueType>::find(const KeyType&key){
    if(key>this->m_tailKey)
        return nullptr;
    // forwardNode是 < key所在节点的最大节点
    skipNode<KeyType,ValueType>*forwardNode=m_headNode;
    for (int i = m_curMaxLevel-1; i >=0 ; --i){
        while (forwardNode->next[i]!=m_tailNode && forwardNode->next[i]->element.key < key){
            forwardNode=forwardNode->next[i];
        }
    }
    // 最终会遍历到第m_curMaxLevel=0层->原始层，只需要在第0层判断即可
    if(forwardNode->next[0]->element.key==key){
        return &forwardNode->next[0]->element;
    }else{
        return nullptr;
    }
}


template<class KeyType,class ValueType>
bool skiplist<KeyType,ValueType>::erase(const KeyType&key){
    // 不符合的key
    if(key>m_tailKey)
        return false;
    skipNode<KeyType,ValueType>*pNode=search(key);
    // 不存在 
    if(pNode->element.key!=key)
        return false;
    // 更新跳表链表结构
    for (int i = m_curMaxLevel-1; i >= 0; --i){
        // 此处 m_forwardNodes[i]->next[i] 可能不是 pNode
        if(m_forwardNodes[i]->next[i]==pNode)
            m_forwardNodes[i]->next[i]=pNode->next[i];
    }
    
    // 维护当前最大层级数
    // 当删除一个具有最大层级的节点时，可能会导致 m_headNode->next[m_maxLevel-1]=m_tailNode，那么此时需要降低层级
    while (m_curMaxLevel-1 > 0&&m_headNode->next[m_curMaxLevel-1]==m_tailNode){
        m_curMaxLevel--;
    }
    delete pNode;   
    m_size--;    
    return true;
}

template<class KeyType,class ValueType>
void skiplist<KeyType,ValueType>::output(){
    if(m_size<=0)return;
    for (int i = m_curMaxLevel-1; i>=0; i--){
        skipNode<KeyType,ValueType> * cur=m_headNode->next[i];
        cout<<"head"<<i<<" => ";
        while (cur!=m_tailNode){
            if(cur==m_headNode->next[i]){
                cout<<cur->element.key<<":"<<cur->element.value;
            }else{
                cout<<"->"<<cur->element.key<<":"<<cur->element.value;
            }
            cur=cur->next[i];
        }
        cout<<endl;
    }
}

#ifndef DYNAMICGRAPH_TEMPLATE_H
#define DYNAMICGRAPH_TEMPLATE_H

#include "DynamicGraph.h"

namespace ttk
{
   namespace ftr
   {

      // DynamicGraph ----------------------------------

      template <typename Type>
      DynamicGraph<Type>::DynamicGraph()
      {
      }

      template <typename Type>
      DynamicGraph<Type>::~DynamicGraph()
      {
      }

      template <typename Type>
      void DynamicGraph<Type>::alloc()
      {
         nodes_.resize(nbVerts_);
      }

      template<typename Type>
      void DynamicGraph<Type>::init()
      {
      }

      template <typename Type>
      int DynamicGraph<Type>::removeEdge(DynGraphNode<Type>* const n1,
                                               DynGraphNode<Type>* const n2)
      {
         if (n1->parent_ == n2) {
            removeEdge(n1);
            return 1;
         }

         if (n2->parent_ == n1) {
            removeEdge(n2);
            return 2;
         }

         return 0;
      }

      template <typename Type>
      void DynamicGraph<Type>::print(void)
      {
         using namespace std;

         for(const auto& node : nodes_) {
            if (node.parent_) {
               cout << "id: " << &node - &nodes_[0] << " weight: " << (float)node.weight_;
               if (node.parent_) {
                  cout << ", parent: " << node.parent_ - &nodes_[0];
               } else {
                  cout << ", parent: X";
               }
               cout << " root: " << findRoot(&node) - &nodes_[0] << endl;
            }
         }
      }

      template <typename Type>
      void DynamicGraph<Type>::print(std::function<std::string(std::size_t)> printFunction)
      {
         using namespace std;

         for(const auto& node : nodes_) {
            if (node.parent_) {
               cout << "id: " << printFunction(&node - &nodes_[0])
                    << " weight: " << (float)node.weight_;
               if (node.parent_) {
                  cout << ", parent: " << printFunction(node.parent_ - &nodes_[0]);
               } else {
                  cout << ", parent: X";
               }
               cout << " root: " << printFunction(findRoot(&node) - &nodes_[0]) << endl;
            }
         }
      }

      template <typename Type>
      void DynamicGraph<Type>::test(void)
      {
         DynamicGraph testGraph;
         testGraph.setNumberOfNodes(4);
         testGraph.alloc();
         testGraph.init();

         DynGraphNode<Type>* n1 = testGraph.getNode(0);
         DynGraphNode<Type>* n2 = testGraph.getNode(1);

         if(testGraph.getNbCC({n1,n2}) != 2){
            std::cerr << "[Dynamic Graph]: bad initial number of CC in test ";
            std::cerr << testGraph.getNbCC({n1, n2}) << " != 2 " << std::endl;
         }

         testGraph.insertEdge(n1, n2, 0);

         // 1 -> 2

         if (testGraph.getNbCC({n1, n2}) != 1) {
            std::cerr << "[Dynamic Graph]: bad number of CC after insert edge ";
            std::cerr << testGraph.getNbCC({n1, n2}) << " != 1 " << std::endl;
         }

         DynGraphNode<Type>* n3 = testGraph.getNode(2);
         DynGraphNode<Type>* n4 = testGraph.getNode(3);

         testGraph.insertEdge(n3, n4, 0);

         // 1 -> 2
         // 3 -> 4

         if (testGraph.getNbCC({n1, n2, n3, n4}) != 2) {
            std::cerr << "[Dynamic Graph]: bad number of CC after insert edge (2) ";
            std::cerr << testGraph.getNbCC({n1, n2, n3, n4}) << " != 2 " << std::endl;
         }

         testGraph.insertEdge(n2, n3, 0);

         // 1 -> 2 -> 3 -> 4

         if (testGraph.getNbCC({n1, n2, n3, n4}) != 1) {
            std::cerr << "[Dynamic Graph]: bad number of CC after insert edge (3) ";
            std::cerr << testGraph.getNbCC({n1, n2, n3, n4}) << " != 1 " << std::endl;
         }

         testGraph.removeEdge(n1);

         // 1
         // 2 -> 3 -> 4

         if (testGraph.getNbCC({n1, n2, n3, n4}) != 2) {
            std::cerr << "[Dynamic Graph]: bad number of CC after remove edge ";
            std::cerr << testGraph.getNbCC({n1, n2, n3, n4}) << " != 2 " << std::endl;
         }

         // replace existing
         testGraph.insertEdge(n2, n4, 1);

         // 1
         // 3 -> 2 -> 4

         if (testGraph.getNbCC({n1, n2, n3, n4}) != 2) {
            std::cerr << "[Dynamic Graph]: bad number of CC after replace edge ";
            std::cerr << testGraph.getNbCC({n1, n2, n3, n4}) << " != 2 " << std::endl;
         }

         testGraph.insertEdge(n1, n4, 0);

         // 3 -> 2 -> 4
         //      1 _/

         if (testGraph.getNbCC({n1, n2, n3, n4}) != 1) {
            std::cerr << "[Dynamic Graph]: bad number of CC after insert edge 4 ";
            std::cerr << testGraph.getNbCC({n1, n2, n3, n4}) << " != 1 " << std::endl;
         }

         testGraph.print();
      }

      // DynGraphNode ----------------------------------

      template <typename Type>
      void DynGraphNode<Type>::evert(void)
      {
         if(!parent_) return;

         DynGraphNode<Type>* curNode      = this;
         Type                curWeight    = curNode->weight_;

         DynGraphNode<Type>* parentNode   = curNode->parent_;
         Type                parentWeight = curNode->weight_;

         DynGraphNode<Type>* gParentNode   = parentNode->parent_;
         Type                gParentWeight = parentNode->weight_;

         curNode->nbChilds_++;
         curNode->parent_ = nullptr;

         // Reverse all the node until the root
         while (true) {
            parentNode->parent_  = curNode;
            parentNode->weight_ = parentWeight;

            curNode      = parentNode;
            parentNode   = gParentNode;
            parentWeight = gParentWeight;

            if (gParentNode) {
               gParentWeight = gParentNode->weight_;
               gParentNode = gParentNode->parent_;
            } else{
               break;
            }
         }
      }

      template <typename Type>
      DynGraphNode<Type>* DynGraphNode<Type>::findRoot(void) const
      {
         DynGraphNode* curNode = const_cast<DynGraphNode<Type>*>(this);
         while (curNode->parent_ != nullptr) {
            curNode = curNode->parent_;
         }
         return curNode;
      }

      template <typename Type>
      std::tuple<DynGraphNode<Type>*, DynGraphNode<Type>*>
      DynGraphNode<Type>::findMinWeightRoot() const
      {
         DynGraphNode* curNode = const_cast<DynGraphNode<Type>*>(this);
         DynGraphNode* minNode = const_cast<DynGraphNode<Type>*>(this);

         while (curNode->parent_) {
            if (curNode->weight_ < minNode->weight_) {
               minNode = curNode;
            }
            curNode = curNode->parent_;
         }
         return std::make_tuple(curNode, minNode);
      }

      template<typename Type>
      bool DynGraphNode<Type>::insertEdge(DynGraphNode<Type>* const n, const Type weight)
      {
         evert();
         auto nNodes = n->findMinWeightRoot();

         if (std::get<0>(nNodes) != this) {
            // The two nodes are in two different trees
            parent_ = n;
            weight_ = weight;
            n->nbChilds_++;
            std::cout << ")) merge trees" << std::endl;
            return true;
         }

         // here the nodes are in the same tree

         if (weight > std::get<1>(nNodes)->weight_) {
            // We need replace this edge by the new one as the current weight is higher

            // add arc
            parent_ = n;
            weight_ = weight;
            n->nbChilds_++;

            // remove old
            std::get<1>(nNodes)->parent_->nbChilds_--;
            std::get<1>(nNodes)->parent_ = 0;
            std::cout << ")) replace arc" << std::endl;
         } else {
            std::cout << ")) nothing" << std::endl;
         }

         return false;
      }

      template <typename Type>
      void DynGraphNode<Type>::removeEdge(void)
      {
#ifndef TTK_ENABLE_KAMIKAZE
         if (!parent_) {
            std::cerr << "[FTR Graph]: DynGraph remove edge in root node" << std::endl;
            return;
         }
#endif

         parent_->nbChilds_--;
         parent_ = nullptr;
      }

   }
}

#endif /* end of include guard: DYNAMICGRAPH_TEMPLATE_H */

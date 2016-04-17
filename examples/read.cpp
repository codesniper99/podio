// STL
#include <vector>
#include <iostream>
#include <exception>

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

// test data model
#include "ExampleHitCollection.h"
#include "ExampleClusterCollection.h"
#include "ExampleReferencingTypeCollection.h"
#include "ExampleWithOneRelationCollection.h"
#include "ExampleWithVectorMemberCollection.h"
#include "ExampleWithComponentCollection.h"
#include "ExampleWithARelationCollection.h"
#include "ExampleWithNamespace.h"

int glob = 0;

void processEvent(podio::EventStore& store, bool verboser) {

  auto& failing = store.get<ExampleClusterCollection>("notthere");
  if(failing.isValid() == true) {
    throw std::runtime_error("Collection 'notthere' should not be valid");
};

  //std::cout << "Fetching collection 'clusters'" << std::endl;
  auto& clusters = store.get<ExampleClusterCollection>("clusters");
  if(clusters.isValid()){
    auto cluster = clusters[0];
    //std::cout << "Cluster has an energy of " << cluster.energy() << std::endl;
    for (auto i = cluster.Hits_begin(), end = cluster.Hits_end(); i!=end; ++i){
      std::cout << "  Referenced hit has an energy of " << i->energy() << std::endl;
      glob++;
    }
  }

  //std::cout << "Fetching collection 'refs'" << std::endl;
  auto& refs = store.get<ExampleReferencingTypeCollection>("refs");
  if(refs.isValid()){
    auto ref = refs[0];
    for (auto j = ref.Clusters_begin(), end = ref.Clusters_end(); j!=end; ++j){
      for (auto i = j->Hits_begin(), end = j->Hits_end(); i!=end; ++i){
        //std::cout << "  Referenced object has an energy of " << i->energy() << std::endl;
        glob++;
      }
    }
  }
  //std::cout << "Fetching collection 'OneRelation'" << std::endl;
  auto& rels = store.get<ExampleWithOneRelationCollection>("OneRelation");
  if(rels.isValid()) {
    //std::cout << "Referenced object has an energy of " << (*rels)[0].cluster().energy() << std::endl;
    glob++;
  }

//  std::cout << "Fetching collection 'WithVectorMember'" << std::endl;
  auto& vecs = store.get<ExampleWithVectorMemberCollection>("WithVectorMember");
  if(vecs.isValid()) {
//    auto item = (*vecs)[0];
//    std::cout << (*vecs).size() << std::endl;
//    for (auto c = item.count_begin(), end = item.count_end(); c!=end; ++c){
//      std::cout << "  Counter value " << (*c) << std::endl;
//      glob++;
//    }
  }

  auto& comps = store.get<ExampleWithComponentCollection>("Component");
  if (comps.isValid()) {
    auto comp = comps[0];
    int a = comp.component().data.x + comp.component().data.z;
  }

  auto& nmspaces = store.get<ex::ExampleWithARelationCollection>("WithNamespaceRelation");
  if (nmspaces.isValid()) {
    auto nmsp = nmspaces[0];
    int b = nmsp.ref().data().x + nmsp.ref().data().y;
  }
}

int main(){
  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  reader.openFile("example.root");
  store.setReader(&reader);

  bool verbose = true;

  unsigned nEvents = reader.getEntries();
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%1000==0)
      std::cout<<"reading event "<<i<<std::endl;
    processEvent(store, true);
    store.clear();
    reader.endOfEvent();
  }
  return 0;
}

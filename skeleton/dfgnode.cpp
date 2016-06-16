#include "dfgnode.h"
#include "dfg.h"

dfgNode::dfgNode(Instruction *ins, DFG* parent){
	this->Node = ins;
	this->Parent = parent;
}

std::vector<Instruction*>::iterator dfgNode::getChildIterator(){
	return Children.begin();
}

std::vector<Instruction*> dfgNode::getChildren(){
	return Children;
}

std::vector<Instruction*> dfgNode::getAncestors(){
	return Ancestors;
}

Instruction* dfgNode::getNode(){
	return Node;
}

void dfgNode::addChild(Instruction *child, int type){

	for (int i = 0; i < Children.size(); ++i) {
		if(child == Children[i]){
			return;
		}
	}

	Children.push_back(child);

	Edge temp;
	temp.setID(Parent->getEdges().size());

	std::ostringstream ss;
	ss << std::hex << static_cast<void*>(Node) << "_to_" << static_cast<void*>(child);
	temp.setName(ss.str());
	temp.setType(type);
	temp.setSrc(Node);
	temp.setDest(child);

	Parent->InsertEdge(temp);
}

int dfgNode::getIdx() {
	return idx;
}

void dfgNode::addAncestor(Instruction *anc, int type){
	for (int i = 0; i < Ancestors.size(); ++i) {
		if(anc == Ancestors[i]){
			return;
		}
	}

	Ancestors.push_back(anc);
}

void dfgNode::setIdx(int Idx) {
	idx = Idx;
}

int dfgNode::removeChild(Instruction* child) {
	Instruction* childRem = NULL;
	for (int i = 0; i < getChildren().size(); ++i) {
		if (child == getChildren()[i]){
			childRem = getChildren()[i];
		}
	}

	if(childRem == NULL){
		return -1;
	}

	Children.erase(std::remove(Children.begin(),Children.end(),childRem),Children.end());
	return 1;
}

int dfgNode::removeAncestor(Instruction* anc) {
	Instruction* ancRem = NULL;
	for (int i = 0; i < getAncestors().size(); ++i) {
		if (anc == getAncestors()[i]){
			ancRem = getAncestors()[i];
		}
	}

	if(ancRem == NULL){
		return -1;
	}

	Ancestors.erase(std::remove(Ancestors.begin(),Ancestors.end(),ancRem),Ancestors.end());
	return 1;
}

void dfgNode::setDFSIdx(int dfsidx) {
	DFSidx = dfsidx;
}

int dfgNode::getDFSIdx() {
	return DFSidx;
}

void dfgNode::setASAPnumber(int n) {
	ASAPnumber = n;
}

void dfgNode::setALAPnumber(int n) {
	ALAPnumber = n;
}

int dfgNode::getASAPnumber() {
	return ASAPnumber;
}

int dfgNode::getALAPnumber() {
	return ALAPnumber;
}

void dfgNode::setSchIdx(int n) {
	schIdx = n;
}

int dfgNode::getSchIdx() {
	return schIdx;
}

std::vector<CGRANode*>* dfgNode::getRoutingLocs() {
	return &routingLocs;
}

void dfgNode::setMappedLoc(CGRANode* cNode) {
	mappedLoc = cNode;
}

CGRANode* dfgNode::getMappedLoc() {
	return mappedLoc;
}

void dfgNode::addRecChild(Instruction* child, int type) {
	for (int i = 0; i < RecChildren.size(); ++i) {
		if(child == RecChildren[i]){
			return;
		}
	}

	RecChildren.push_back(child);

	Edge temp;
	temp.setID(Parent->getEdges().size());

	std::ostringstream ss;
	ss << std::hex << static_cast<void*>(Node) << "_to_" << static_cast<void*>(child);
	temp.setName(ss.str());
	temp.setType(type);
	temp.setSrc(Node);
	temp.setDest(child);

	Parent->InsertEdge(temp);
}

void dfgNode::addRecAncestor(Instruction* anc, int type) {
	for (int i = 0; i < RecAncestors.size(); ++i) {
		if(anc == RecAncestors[i]){
			return;
		}
	}

	RecAncestors.push_back(anc);
}

int dfgNode::removeRecChild(Instruction* child) {
	Instruction* childRem = NULL;
	for (int i = 0; i < getRecChildren().size(); ++i) {
		if (child == getRecChildren()[i]){
			childRem = getRecChildren()[i];
		}
	}

	if(childRem == NULL){
		return -1;
	}

	RecChildren.erase(std::remove(RecChildren.begin(),RecChildren.end(),childRem),RecChildren.end());
	return 1;
}

int dfgNode::removeRecAncestor(Instruction* anc) {
	Instruction* ancRem = NULL;
	for (int i = 0; i < getRecAncestors().size(); ++i) {
		if (anc == getRecAncestors()[i]){
			ancRem = getRecAncestors()[i];
		}
	}

	if(ancRem == NULL){
		return -1;
	}

	RecAncestors.erase(std::remove(RecAncestors.begin(),RecAncestors.end(),ancRem),RecAncestors.end());
	return 1;
}

void dfgNode::addPHIchild(Instruction* child, int type) {
	PHIchildren.push_back(child);

	Edge temp;
	temp.setID(Parent->getEdges().size());

	std::ostringstream ss;
	ss << std::hex << static_cast<void*>(Node) << "_to_" << static_cast<void*>(child);
	temp.setName(ss.str());
	temp.setType(type);
	temp.setSrc(Node);
	temp.setDest(child);

	Parent->InsertEdge(temp);
}

void dfgNode::addPHIancestor(Instruction* anc, int type) {
	for (int i = 0; i < PHIAncestors.size(); ++i) {
		if(anc == PHIAncestors[i]){
			return;
		}
	}

	PHIAncestors.push_back(anc);
}

std::map<dfgNode*, std::vector<CGRANode*> > dfgNode::getMergeRoutingLocs() {
	std::map<dfgNode*, std::vector<CGRANode*> > temp = treeBasedRoutingLocs;

	std::map<dfgNode*, std::vector<CGRANode*> >::iterator tempIt;
	for(tempIt = temp.begin();
		tempIt != temp.end();
		tempIt++){

		if(treeBasedGoalLocs.find(tempIt->first) != treeBasedGoalLocs.end()){
			for (int i = 0; i < treeBasedGoalLocs[tempIt->first].size(); ++i) {
				temp[tempIt->first].insert(temp[tempIt->first].begin(),treeBasedGoalLocs[tempIt->first][i]);
			}
		}
	}

	return temp;
}
#include "dfgnode.h"
#include "dfg.h"

dfgNode::dfgNode(Instruction *ins, DFG* parent){
	this->Node = ins;
	this->BB = ins->getParent();
	this->Parent = parent;

	for (int i = 0; i < ins->getNumOperands(); ++i) {
		if(ConstantInt *CI = dyn_cast<ConstantInt>(ins->getOperand(i))){

			if(dyn_cast<PHINode>(ins)){
				break;
			}

			if(this->hasConstantVal()){
				ins->dump();
			}
			assert(this->hasConstantVal()==false);
			this->setConstantVal(CI->getSExtValue());
		}
	}

	if(dyn_cast<LoadInst>(ins)){
		this->setTypeSizeBytes(ins->getType()->getIntegerBitWidth()/8);
	}

	if(StoreInst* SI = dyn_cast<StoreInst>(ins)){
		this->setTypeSizeBytes(SI->getValueOperand()->getType()->getIntegerBitWidth()/8);
	}
}

std::vector<Instruction*>::iterator dfgNode::getChildIterator(){
	return Children.begin();
}

Instruction* dfgNode::getNode(){
	return Node;
}

int dfgNode::getIdx() {
	return idx;
}

void dfgNode::addChild(Instruction *child, int type){

	for (int i = 0; i < Children.size(); ++i) {
		if(child == Children[i]){
			return;
		}
	}

	Children.push_back(child);
}

void dfgNode::addAncestor(Instruction *anc, int type){
	assert(anc != NULL);
	errs() << "Ancestor Size = " << this->Ancestors.empty() << "\n";
	for (int i = 0; i < Ancestors.size(); ++i) {
		if(anc == Ancestors[i]){
			return;
		}
	}

	Ancestors.push_back(anc);
	dfgNode* ancNode = Parent->findNode(anc);
	ancNode->addChildNode(this);
	AncestorNodes.push_back(ancNode);

	Edge temp;
	temp.setID(Parent->getEdges().size());

	std::ostringstream ss;
	ss << std::dec << ancNode->getIdx() << "_to_" << this->getIdx();
	temp.setName(ss.str());
	temp.setType(type);
	temp.setSrc(ancNode);
	temp.setDest(this);

	Parent->InsertEdge(temp);
}

void dfgNode::addChildNode(dfgNode* node, int type) {
	ChildNodes.push_back(node);
}

void dfgNode::addAncestorNode(dfgNode* node, int type) {
	AncestorNodes.push_back(node);
	Edge temp;
	temp.setID(Parent->getEdges().size());

	std::ostringstream ss;
	ss << std::dec << node->getIdx() << "_to_" << this->getIdx();
	temp.setName(ss.str());
	temp.setType(type);
	temp.setSrc(node);
	temp.setDest(this);

	Parent->InsertEdge(temp);
}

void dfgNode::addPHIAncestorNode(dfgNode* node) {
	assert(node != NULL);
	PHIAncestorNodes.push_back(node);

	Edge temp;
	temp.setID(Parent->getEdges().size());

	std::ostringstream ss;
	ss << std::dec << node->getIdx() << "_to_" << this->getIdx();
	temp.setName(ss.str());
	temp.setType(EDGE_TYPE_PHI);
	temp.setSrc(node);
	temp.setDest(this);

	Parent->InsertEdge(temp);
}


void dfgNode::setIdx(int Idx) {
	idx = Idx;
}

int dfgNode::removeChild(Instruction* child) {
	Instruction* childRem = NULL;
	for (int i = 0; i < getChildren().size(); ++i) {
		if (child == getChildren()[i]->getNode()){
			childRem = getChildren()[i]->getNode();
		}
	}

	if(childRem == NULL){
		return -1;
	}

	Children.erase(std::remove(Children.begin(),Children.end(),childRem),Children.end());
	return 1;
}

int dfgNode::removeChild(dfgNode* child) {
	assert(child != NULL);
	ChildNodes.erase(std::remove(ChildNodes.begin(),ChildNodes.end(),child),ChildNodes.end());
	return 1;
}

int dfgNode::removeAncestor(Instruction* anc) {
	Instruction* ancRem = NULL;
	for (int i = 0; i < getAncestors().size(); ++i) {
		if (anc == getAncestors()[i]->getNode()){
			ancRem = getAncestors()[i]->getNode();
		}
	}

	if(ancRem == NULL){
		return -1;
	}

	Ancestors.erase(std::remove(Ancestors.begin(),Ancestors.end(),ancRem),Ancestors.end());
	return 1;
}

int dfgNode::removeAncestor(dfgNode* anc) {
	assert(anc != NULL);
	AncestorNodes.erase(std::remove(AncestorNodes.begin(),AncestorNodes.end(),anc),AncestorNodes.end());
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

//	Edge temp;
//	temp.setID(Parent->getEdges().size());
//
//	std::ostringstream ss;
//	ss << std::hex << static_cast<void*>(Node) << "_to_" << static_cast<void*>(child);
//	temp.setName(ss.str());
//	temp.setType(type);
//	temp.setSrc(Node);
//	temp.setDest(child);
//
//	Parent->InsertEdge(temp);
}

void dfgNode::addRecAncestor(Instruction* anc, int type) {
	for (int i = 0; i < RecAncestors.size(); ++i) {
		if(anc == RecAncestors[i]){
			return;
		}
	}

	RecAncestors.push_back(anc);

	dfgNode* recAncNode = Parent->findNode(anc);
	recAncNode->addRecChildNode(this);
	RecAncestorNodes.push_back(recAncNode);

	Edge temp;
	temp.setID(Parent->getEdges().size());

	std::ostringstream ss;
	ss << std::dec << recAncNode->getIdx() << "_to_" << this->getIdx();
	temp.setName(ss.str());
	temp.setType(type);
	temp.setSrc(recAncNode);
	temp.setDest(this);

	Parent->InsertEdge(temp);
}

int dfgNode::removeRecChild(Instruction* child) {
	Instruction* childRem = NULL;
	for (int i = 0; i < getRecChildren().size(); ++i) {
		if (child == getRecChildren()[i]->getNode()){
			childRem = getRecChildren()[i]->getNode();
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
		if (anc == getRecAncestors()[i]->getNode()){
			ancRem = getRecAncestors()[i]->getNode();
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

//	Edge temp;
//	temp.setID(Parent->getEdges().size());
//
//	std::ostringstream ss;
//	ss << std::dec << this->getIdx() << "_to_" << Parent->findNode(child)->getIdx();
//	temp.setName(ss.str());
//	temp.setType(type);
//	temp.setSrc(this);
//	temp.setDest(Parent->findNode(child));
//
//	Parent->InsertEdge(temp);
}

void dfgNode::addPHIancestor(Instruction* anc, int type) {
	errs() << "test\n";
	for (int i = 0; i < PHIAncestors.size(); ++i) {
		if(anc == PHIAncestors[i]){
			return;
		}
	}
	errs() << "test\n";
	PHIAncestors.push_back(anc);

	dfgNode* phiAncNode = Parent->findNode(anc);
	phiAncNode->addPHIChildNode(this);
	PHIAncestorNodes.push_back(phiAncNode);

	Edge temp;
	temp.setID(Parent->getEdges().size());

	std::ostringstream ss;
	ss << std::dec << phiAncNode->getIdx() << "_to_" << this->getIdx();
	temp.setName(ss.str());
	temp.setType(type);
	temp.setSrc(phiAncNode);
	temp.setDest(this);

	Parent->InsertEdge(temp);
}

bool pathPredictCompare(pathData* p1, pathData* p2){
	return (p1->cnode == p2->cnode);
}

std::map<dfgNode*,std::vector<pathData*>> dfgNode::getMergeRoutingLocs() {
	std::map<dfgNode*,std::vector<pathData* >> temp = treeBasedRoutingLocs;

	std::vector<pathData*> searchList;

	std::map<dfgNode*,std::vector<pathData* >>::iterator tempIt;
	for(tempIt = temp.begin();
		tempIt != temp.end();
		tempIt++){


		if(treeBasedGoalLocs.find(tempIt->first) != treeBasedGoalLocs.end()){
			for (int i = 0; i < treeBasedGoalLocs[tempIt->first].size(); ++i) {
				searchList.clear();
				searchList.push_back(treeBasedGoalLocs[tempIt->first][i]);

				if(std::search(temp[tempIt->first].begin(),
							   temp[tempIt->first].end(),
							   searchList.begin(),
							   searchList.end(),
							   pathPredictCompare) == temp[tempIt->first].end()){

					temp[tempIt->first].insert(temp[tempIt->first].begin(),treeBasedGoalLocs[tempIt->first][i]);

				}

//				if(std::find(temp[tempIt->first].begin(),
//						     temp[tempIt->first].end(),
//							 treeBasedGoalLocs[tempIt->first][i]) == temp[tempIt->first].end()){
//					temp[tempIt->first].insert(temp[tempIt->first].begin(),treeBasedGoalLocs[tempIt->first][i]);
//				}
			}
		}

	}

	return temp;
}


bool dfgNode::isConditional() {
	if(this->getNode() != NULL){
		if(this->getNode()->getOpcode() == Instruction::Br){
			return true;
		}
	}
	else if(this->getNameType().compare("CTRLBrOR") == 0){
		return true;
	}
	return false;
}

void dfgNode::addStoreChild(Instruction * ins) {
	dfgNode* temp;


	if(Parent->OutLoopNodeMap.find(ins) == Parent->OutLoopNodeMap.end()){
		temp = new dfgNode(Parent);
		temp->setNameType("OutLoopSTORE");
		temp->setIdx(Parent->getNodesPtr()->size());
		Parent->getNodesPtr()->push_back(temp);
		Parent->OutLoopNodeMap[ins] = temp;
		Parent->OutLoopNodeMapReverse[temp]=ins;

		if(Parent->accumulatedBBs.find(ins->getParent())==Parent->accumulatedBBs.end()){
			temp->setTransferedByHost(true);
		}
	}
	else{
		temp = Parent->OutLoopNodeMap[ins];
		return;
	}

	temp->addAncestorNode(this);
	this->addChildNode(temp);

	temp->BB = this->getNode()->getParent();

	//ceiling upto multiple of 8
	outs() << "addStoreChild : ins="; ins->dump();
	temp->setTypeSizeBytes((ins->getType()->getIntegerBitWidth()+7)/8);

}

void dfgNode::addLoadParent(Instruction * ins) {
	dfgNode* temp;


	if(Parent->OutLoopNodeMap.find(ins) == Parent->OutLoopNodeMap.end()){
		temp = new dfgNode(Parent);
		temp->setNameType("OutLoopLOAD");
		temp->setIdx(Parent->getNodesPtr()->size());
		Parent->getNodesPtr()->push_back(temp);
		Parent->OutLoopNodeMap[ins] = temp;
		Parent->OutLoopNodeMapReverse[temp]=ins;

		if(Parent->accumulatedBBs.find(ins->getParent())==Parent->accumulatedBBs.end()){
			temp->setTransferedByHost(true);
		}
	}
	else{
		temp = Parent->OutLoopNodeMap[ins];
	}

	this->addAncestorNode(temp);
	temp->addChildNode(this);

	temp->BB = this->BB;

//	if(ins->getType()->getIntegerBitWidth() < 8){
//		ins->dump();
//		errs() << "integerBitWidth = " << ins->getType()->getIntegerBitWidth() << "\n";
//	}
//	assert(ins->getType()->getIntegerBitWidth() >= 8);

	//ceiling upto multiple of 8
	outs() << "addLoadParent :" << ",node =" << temp->getIdx() << ",for the node=" << this->getIdx() << "\n";
	ins->dump();

	if(GetElementPtrInst* GEP = dyn_cast<GetElementPtrInst>(ins)){
		temp->setTypeSizeBytes(4);
	}
	else if(AllocaInst* ALOCA = dyn_cast<AllocaInst>(ins)){
		errs() << "ALOCA\n";
		temp->setTypeSizeBytes(4);
	}
	else{
		temp->setTypeSizeBytes((ins->getType()->getIntegerBitWidth()+7)/8);
	}


}

int dfgNode::getoutloopAddr() {
	//should be outerloop load and store instruction
	assert((this->getNameType().compare("OutLoopLOAD") == 0)||(this->getNameType().compare("OutLoopSTORE") == 0));
	return outloopAddr;
}

void dfgNode::setoutloopAddr(int addr) {
	assert((this->getNameType().compare("OutLoopLOAD") == 0)||(this->getNameType().compare("OutLoopSTORE") == 0));
	outloopAddr = addr;
	constValFlag = true;
}

int dfgNode::getGEPbaseAddr() {
	assert(this->getNode());
	assert(dyn_cast<GetElementPtrInst>(this->getNode()));
	return GEPbaseAddr;
}

void dfgNode::setGEPbaseAddr(int addr) {
	assert(this->getNode());
	assert(dyn_cast<GetElementPtrInst>(this->getNode()));
	GEPbaseAddr = addr;
	constValFlag = true;
}

dfgNode* dfgNode::addCMergeParent(dfgNode* phiBRAncestor,dfgNode* phiDataAncestor,int32_t constVal,bool selectOp) {
	dfgNode* temp = new dfgNode(Parent);
	temp->setIdx(Parent->getNodesPtr()->size());
	Parent->getNodesPtr()->push_back(temp);
	temp->BB = this->BB;
	temp->setNameType("CMERGE");

	if(phiDataAncestor==NULL){
		temp->setConstantVal(constVal);
	}
	else{
		temp->addAncestorNode(phiDataAncestor);
		phiDataAncestor->addChildNode(temp);

	}

	temp->addAncestorNode(phiBRAncestor);
	phiBRAncestor->addChildNode(temp);

	if(selectOp){
		temp->addChildNode(this);
		this->addAncestorNode(temp);
	}
	else{
		temp->addPHIChildNode(this);
		this->addPHIAncestorNode(temp);
	}

	return temp;
}

dfgNode* dfgNode::addCMergeParent(dfgNode* phiBRAncestor,
		Instruction* outLoopLoadIns) {

	dfgNode* temp = new dfgNode(Parent);
	temp->setIdx(Parent->getNodesPtr()->size());
	Parent->getNodesPtr()->push_back(temp);
	temp->setNameType("CMERGE");
	temp->BB = this->BB;

	temp->addLoadParent(outLoopLoadIns);


	temp->addAncestorNode(phiBRAncestor);
	phiBRAncestor->addChildNode(temp);

	temp->addPHIChildNode(this);
	this->addPHIAncestorNode(temp);

	return temp;
}

bool dfgNode::isOutLoop() {
	return (this->getNameType().compare("OutLoopLOAD") == 0)||(this->getNameType().compare("OutLoopSTORE") == 0);
}

bool dfgNode::isGEP() {
	if (this->getNode() == NULL) return false;
	if (dyn_cast<GetElementPtrInst>(this->getNode())){
		return true;
	}
	else{
		return false;
	}
}

dfgNode* dfgNode::addCMergeParent(Instruction* phiBRAncestorIns,
		Instruction* outLoopLoadIns) {

	dfgNode* temp = new dfgNode(Parent);
	temp->setIdx(Parent->getNodesPtr()->size());
	Parent->getNodesPtr()->push_back(temp);
	temp->setNameType("CMERGE");
	temp->BB = this->BB;

	temp->addLoadParent(outLoopLoadIns);

	assert(phiBRAncestorIns!=NULL);
	dfgNode* tempBR;
	if(Parent->LoopStartMap.find(phiBRAncestorIns)!=Parent->LoopStartMap.end()){
		tempBR = Parent->LoopStartMap[phiBRAncestorIns];
	}
	else{
		tempBR = new dfgNode(Parent);
		tempBR->setIdx(Parent->getNodesPtr()->size());
		Parent->getNodesPtr()->push_back(tempBR);
		tempBR->setNameType("LOOPSTART");
		tempBR->BB = this->BB;
		Parent->LoopStartMap[phiBRAncestorIns]=tempBR;
	}


	temp->addAncestorNode(tempBR);
	tempBR->addChildNode(temp);

	temp->addPHIChildNode(this);
	this->addPHIAncestorNode(temp);

	return temp;
}

dfgNode* dfgNode::addCMergeParent(Instruction* phiBRAncestorIns, int32_t constVal) {
	dfgNode* temp = new dfgNode(Parent);
	temp->setIdx(Parent->getNodesPtr()->size());
	Parent->getNodesPtr()->push_back(temp);
	temp->setNameType("CMERGE");
	temp->BB = this->BB;

	temp->setConstantVal(constVal);


	assert(phiBRAncestorIns!=NULL);
	dfgNode* tempBR;
	if(Parent->LoopStartMap.find(phiBRAncestorIns)!=Parent->LoopStartMap.end()){
		tempBR = Parent->LoopStartMap[phiBRAncestorIns];
	}
	else{
		tempBR = new dfgNode(Parent);
		tempBR->setIdx(Parent->getNodesPtr()->size());
		Parent->getNodesPtr()->push_back(tempBR);
		tempBR->setNameType("LOOPSTART");
		tempBR->BB = this->BB;
		Parent->LoopStartMap[phiBRAncestorIns]=tempBR;
	}


	temp->addAncestorNode(tempBR);
	tempBR->addChildNode(temp);

	temp->addPHIChildNode(this);
	this->addPHIAncestorNode(temp);

	return temp;
}

void dfgNode::setFinalIns(HyCUBEIns ins){
	outs() <<  "SetFinalIns::Node=" << this->getIdx() << ",HyIns=" << Parent->HyCUBEInsStrings[ins] << "\n";
	finalIns = ins;
}

bool dfgNode::isParent(dfgNode* parent) {
	if(this->getAncestors().empty()){
		return false;
	}

	if(parent==NULL){
		return false;
	}

	for(dfgNode* par : this->getAncestors()){
		if(par == parent){
			return true;
		}
	}

	return false;
}

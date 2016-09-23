// UD1414_Plugin.cpp : Defines the exported functions for the DLL application.

#include "MayaIncludes.h"
#include "HelloWorldCmd.h"
#include <iostream>
using namespace std;

MCallbackIdArray ids;

void* HelloWorld::creator() { 
	return new HelloWorld;
};
MStatus HelloWorld::doIt(const MArgList& argList)
{
	MGlobal::displayInfo("Hello World!");
	return MS::kSuccess;
};

void onNodeCreate(MObject& node, void *clientData)
{
	
	//if(node.hasFn(MFn::kTransform))
	//	MFnTransform transMFn(node);
	//MFnDagNode dagMFn(node);
	//MDagPath lol;
	//MStatus res;
	//lol = MDagPath::getAPathTo(node, &res);
	//
	//if(res.kSuccess)
	//	const MString talihu = lol.fullPathName(&res);
	//if (res.kSuccess)
	//{
	//	MGlobal::displayInfo(talihu);
	//}
	////dagMFn.getPath()
	MGlobal::displayInfo(MFnDependencyNode(node).name() + "Allahu");
}

// called when the plugin is loaded
EXPORT MStatus initializePlugin(MObject obj)
{
	// most functions will use this variable to indicate for errors
	MStatus res = MS::kSuccess;

	MFnPlugin myPlugin(obj, "Maya plugin", "1.0", "Any", &res);
	if (MFAIL(res)) {
		CHECK_MSTATUS(res);
	}
	MStatus status = myPlugin.registerCommand("helloWorld", HelloWorld::creator);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	MGlobal::displayInfo("Maya plugin loaded!");
	// if res == kSuccess then the plugin has been loaded,
	// otherwise it has not.

	MCallbackId temp;
	temp = MDGMessage::addNodeAddedCallback(onNodeCreate,
		"dependNode",
		NULL,
		&res
	);
	if (res.kSuccess)
		ids.append(temp);

	return res;
}


EXPORT MStatus uninitializePlugin(MObject obj)
{
	// simply initialize the Function set with the MObject that represents
	// our plugin
	MFnPlugin plugin(obj);

	// if any resources have been allocated, release and free here before
	// returning...

	MMessage::removeCallbacks(ids);

	MStatus status = plugin.deregisterCommand("helloWorld");
	CHECK_MSTATUS_AND_RETURN_IT(status);
	MGlobal::displayInfo("Maya plugin unloaded!");

	return MS::kSuccess;
}

//int awesomeFunction()
//{
//	return 69;
//}
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

void onNodeAttrChange(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData)
{
    //MGlobal::displayInfo("Node name changed to: " + MFnDependencyNode(node).name());
    MGlobal::displayInfo("Node attribute changed!");
    if (msg == MNodeMessage::AttributeMessage::kAttributeRenamed)
    {
        MGlobal::displayInfo("RenameInfo: " + plug.info());
    }
}

void onNodeCreate(MObject& node, void *clientData)
{
    MStatus res;
    //MFnMesh mesh(node, &res);
    MFnMesh meshFn(node, &res);
    if (res == MStatus::kSuccess)
    {
        MGlobal::displayInfo("CREATE: Mesh name: " + meshFn.name());

        //MCallbackId id = MNodeMessage::addAttributeChangedCallback(node, onNodeAttrChange, NULL, &res);
        //if (res == MStatus::kSuccess)
        //    ids.append(id);
    }
    else
    {
        MFnDagNode dagFn(node, &res);
        if (res = MStatus::kSuccess)
        {
            MGlobal::displayInfo("CREATE: Node name: " + dagFn.name() + " Node type: " + node.apiTypeStr());
        }
    }

   /* if (node.hasFn(MFn::kTransform))
    {
        MGlobal::displayInfo("Node transform: " + MFnTransform(node).name());

        MCallbackId id = MNodeMessage::addAttributeChangedCallback(node, onNodeAttrChange, NULL, &res);
        if (res == MStatus::kSuccess)
            ids.append(id);
    }*/
	//MGlobal::displayInfo("A node was created: " + MFnDependencyNode(node).name());
}

void onNodeNameChange(MObject &node, const MString &str, void *clientData)
{
    MStatus res = MStatus::kSuccess;

    MFnTransform transFn(node, &res);
    if (res == MStatus::kSuccess)
    {
        MGlobal::displayInfo("NEW NAME: New transform node name: " + transFn.name() + " Node Type: " + node.apiTypeStr());
    }
    else
    {
        MFnMesh meshFn(node, &res);

        if (res == MStatus::kSuccess)
        {
            MGlobal::displayInfo("NEW NAME: New mesh node name: " + meshFn.name() + " Node Type: " + node.apiTypeStr());
        }
        else
        {
            MFnDagNode dagFn(node, &res);
            if (res == MStatus::kSuccess)
            {
                //MFnDagNode dagFn(node);
                MGlobal::displayInfo("NEW NAME: New node name: " + dagFn.name() + " Node Type: " + node.apiTypeStr());
            }
        }
    }
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
		kDefaultNodeType,
		NULL,
		&res
	);
    if (res == MS::kSuccess)
    {
        MGlobal::displayInfo("nodeAdded success!");
        ids.append(temp);
    }
    temp = MNodeMessage::addNameChangedCallback(MObject::kNullObj, onNodeNameChange, NULL, &res);
    if (res == MStatus::kSuccess)
    {
        MGlobal::displayInfo("nameChange success!");
        ids.append(temp);
    }
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
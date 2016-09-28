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
/*
Only seems to trigger when new vertices and/or edge loops are added.
*/
void onMeshTopoChange(MObject &node, void *clientData)
{
	MGlobal::displayInfo("TOPOLOGY!");
}

void onNodeAttrChange(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData)
{
    //MGlobal::displayInfo("Node name changed to: " + MFnDependencyNode(node).name());
    //MGlobal::displayInfo("Node attribute changed!");
    //if (msg == MNodeMessage::AttributeMessage::kAttributeSet && !plug.isArray())
	//MGlobal::displayInfo("PlugInfo: ");
	//if (msg == MNodeMessage::AttributeMessage::kAttributeSet)
	//{
 //       MGlobal::displayInfo("PlugInfo: " + plug.info());
 //   }
	//if (msg == MNodeMessage::AttributeMessage::kAttributeEval)
	//{
	//	MGlobal::displayInfo("PlugInfo: " + plug.info());
	//}

	//MGlobal::displayInfo("No message!" + msg);
	//MGlobal::displayInfo("PlugInfo: " + plug.info() + " Otherplug Info: " + otherPlug.info());

	/*
	Hum... Maybe I should make a separate callback function that only gets applied to meshes? 
	*/

	if (msg & MNodeMessage::AttributeMessage::kAttributeSet && !plug.isArray() && plug.isElement())
	{
		MStatus res;
		MObject obj = plug.node(&res);; //this by itself returns kDoubleLinearAttribute

		if (res == MStatus::kSuccess)
		{
			//MGlobal::displayInfo("Success!");
			MFnTransform transFn(obj, &res);
			if (res == MStatus::kSuccess)
			{
				MGlobal::displayInfo("Transform node: " + transFn.name() + " ATTR SET!");
			}
			else
			{
				MFnMesh meshFn(obj, &res);
				if (res == MStatus::kSuccess)
				{
					MPoint aPoint;
					res = meshFn.getPoint(plug.logicalIndex(), aPoint, MSpace::kObject);
					if (res == MStatus::kSuccess)
					{
						aPoint.x;
						MGlobal::displayInfo(MString("Mesh node: ") + meshFn.name() + MString(" ATTR SET! asdasd ") + aPoint.x + " " +  aPoint.y + " " + aPoint.z);
					}
				}
				else
				{
					MFnDagNode dagFn(obj, &res);
					if (res == MStatus::kSuccess)
					{
						MGlobal::displayInfo("Node type: " + dagFn.type() + MString(" node name: ") + dagFn.name() + " ATTR SET!");
					}
				}
			}
		}
	}
}

void onNodeAttrAddedRemoved(MNodeMessage::AttributeMessage msg, MPlug &plug, void *clientData)
{
	MGlobal::displayInfo("Attribute Added or Removed! " + plug.info());
}

void onNodeCreate(MObject& node, void *clientData)
{
    MStatus res;
    //MFnMesh mesh(node, &res);
    MFnMesh meshFn(node, &res);
	if (res == MStatus::kSuccess)
    {
        MGlobal::displayInfo("CREATE: Mesh name: " + meshFn.name());

        MCallbackId id = MNodeMessage::addAttributeChangedCallback(node, onNodeAttrChange, NULL, &res);
        if (res == MStatus::kSuccess)
            ids.append(id);
		id = MNodeMessage::addAttributeAddedOrRemovedCallback(node, onNodeAttrAddedRemoved, NULL, &res);
		if (res == MStatus::kSuccess)
			ids.append(id);
    }
    else
    {
        MFnDagNode dagFn(node, &res);
        if (res = MStatus::kSuccess)
        {
            MGlobal::displayInfo("CREATE: Node name: " + dagFn.name() + " Node type: " + node.apiTypeStr());
			
			MCallbackId id = MNodeMessage::addAttributeChangedCallback(node, onNodeAttrChange, NULL, &res);
			if (res == MStatus::kSuccess)
				ids.append(id);
			id = MNodeMessage::addAttributeAddedOrRemovedCallback(node, onNodeAttrAddedRemoved, NULL, &res);
			if (res == MStatus::kSuccess)
				ids.append(id);
		}
    }

	if (node.hasFn(MFn::kMesh))
	{
		MCallbackId id = MPolyMessage::addPolyTopologyChangedCallback(node, onMeshTopoChange, NULL, &res);
		if (res == MStatus::kSuccess)
		{
			ids.append(id);
			MGlobal::displayInfo("HEYEYE");
		}
	}

    if (node.hasFn(MFn::kTransform))
    {
        MGlobal::displayInfo("Node transform: " + MFnTransform(node).name());

        MCallbackId id = MNodeMessage::addAttributeChangedCallback(node, onNodeAttrChange, NULL, &res);
		if (res == MStatus::kSuccess)
		{
			ids.append(id);
		}
	}
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

![](img.png)
----

# Object Definition files

## What are Object Definition files?

Object Definition files may be used to load object and resource definitions into the Client or Server daemon. This is useful when object definitions need to be in place before the client daemon connects to a LWM2M bootstrap server or LWM2M server, or before clients register with the LWM2M server. The files are loaded at the time the daemon starts.

*Note:* it is not possible to use this feature to extend objects that are already defined, for example by attempting to add resources to an existing object. Only new objects can be defined at this time.

## How are Object Definition files used?

The client or server daemon can load an object definition file with the `--objDefs/-o` option, for example:

    $ awa_serverd --objDefs myObjects.xml
    $ awa_clientd --bootstrap coap://bootstrap:15683 --objDefs myObjects.xml

The bootstrap server cannot load object definition files, nor does it need to.

## What do Object Definition files look like?

Object Definition files are XML files that begin with either an `<ObjectDefinition>` tag or an `<ObjectDefinitions>` tag.

Note: All XML tags and values are case sensitive!

To define a single object and any accompanying resources, the `<ObjectDefinition>` tag is used. For example, to define a new object with ID 7301, containing a single string resource: 

	<ObjectDefinition>
	  <ObjectID>7301</ObjectID>
	  <Name>My Object 1</Name>
	  <SerialisationName>MyObject1</SerialisationName>
	  <Singleton>True</Singleton>
	  <IsMandatory>False</IsMandatory>
	  <Properties>
	    <PropertyDefinition>
	      <PropertyID>0</PropertyID>
	      <Name>My Resource 0</Name>
	      <SerialisationName>MyResource0</SerialisationName>
	      <DataType>String</DataType>
	      <IsCollection>False</IsCollection>
	      <IsMandatory>False</IsMandatory>
	      <Access>ReadWrite</Access>
	    </PropertyDefinition>
	  </Properties>
	</ObjectDefinition>

To define multiple objects, each `<ObjectDefinition>` tag should be stored within this `<ObjectDefinitions>` structure:

	<ObjectDefinitions>
	  <Items>
	    <!-- define object definitions here -->
	    <ObjectDefinition>
	      ...
	    </ObjectDefinition>
	    <ObjectDefinition>
	      ...
	    </ObjectDefinition>
	  ...
	  </Items>
	</ObjectDefinitions>

Each object definition contains the following:

 * `<ObjectID>` specifies the LWM2M object ID for the new object. An object with this ID must not already exist.
 * `<Name>` is currently ignored, however it should be supplied for future use. 
 * `<SerialisationName>` is used as the object name. 
 * `<Singleton>` if `"True"` specifies that only one instance of the object is permitted. If not `"True"`, multiple instances of this object are permitted.
 * `<IsMandatory>` if `"True"` specifies that at least one instance of the object must exist at all times. If not `"True"`, it is permitted to have no instances.

Within each object definition there may be zero or more resource definitions, within the `<Properties>` tag. Each property is wrapped in a `<PropertyDefinition>` tag and should contain the following subtags:

 * `<PropertyID>` specifies the LWM2M resource ID for the new resource.
 * `<Name>` is currently ignored, however it should be supplied for future use.
 * `<SerialisationName>` is used as the resource name.
 * `<DataType>` specifies the LWM2M data type for this resource. Supported values are:
   * `"None"`, `"String"`, `"Integer"`, `"Float"`, `"Boolean"`, `"Opaque"`, `"DateTime"`, `"ObjectLink"` 
 * `<Access` specifies the LWM2M access level for this resource. Supported values are:
   * `"None"`, `"Read"`, `"Write"`, `"ReadWrite"`, `"Execute"`
 * `<IsCollection>` if `"True"` specifies that this resource consists of multiple instances (an array). If not `"True"`, the resource has a single instance.
 * `<IsMandatory>` if `"True"` specifies that the resource must exist at all times. If not `"True"`, it is permitted to be absent.
 

## How can I create an Object Definition file?

Object Definition files can be handcrafted, or alternatively they can be generated by the `awa-client-explore` tool:

    $ awa-client-explore --export myObjDefs.xml

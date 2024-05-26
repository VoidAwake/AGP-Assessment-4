# AGP Assessment 4 Report

Jack Coggins, Francisco Castro

![[3.PNG]]
## Assessment 3 Contributions

### Jack

For this project, I implemented the [Wave Function Collapse Algorithm](https://github.com/mxgmn/WaveFunctionCollapse) developed by [Maxim Gumin](https://github.com/mxgmn). The algorithm generates random structures using a set of tiles and rules about which tiles can be adjacent to each other.

The structure is made up of a grid of cells, each representing a point in space where a tile can eventually occupy. Each cell has a list of tiles that it could be, called a wave. Each tile has rules about what neighbours can go next to it, which are used to restrict the waves of neighbouring cells. At the start of the algorithm, the wave of each cell contains every tile, meaning that it could eventually contain any of them.

The algorithm starts by picking a random cell and generating a random tile from its wave. This is called an observation, as it causes the superposition of the wave to collapse to a single value. Now that a tile has been decided, the rules of that tile restrict the types of tiles that could be placed in neighbouring cells, which restricts their neighbours and so on. These restrictions propagate through the grid, until it reaches a new steady state. 

The algorithm alternates between observation and propagation until the structure is complete. WFC can be used to generate buildings, landscapes and even cities, anything that can be expressed as a set of adjacency rules between tiles. Below is an example of my own WFC implementation being used to generate a simple pipe maze.

![[1.png]]

### Francisco

For this project, I added “GravityFields”; these gravity fields, when entered, will manipulate the player's gravity to move towards the field's direction. The gravity fields can be procedurally generated with the algorithm creating the field until an object/wall is detected to which it will stop generating.

![[0.png]]

Gravity Fields have different modes and types determined by which direction is desired being either horizontal or vertical and either have a push or pull influence to them. Gravity Fields also have another generation type that allows for the user to use a more simpler version of the procedural generation by specifying the mesh’s size instead of using the aforementioned algorithm.

## What Was Planned

The goal of this project was to combine the systems we developed in Assessment 3 and create a polished tech-demo. Our goal was to create a simple, fast-paced first person shooter in the style of Quake or Unreal Tournament. We aimed to use the Wave Function Collapse to procedurally generate maps, using the unique geometry and gravity fields to place an emphasis on vertical positioning and movement.

The use of both the gravity fields and wave function collapse allows for a multitude of different and creative ways of developing maps and allow for more interesting choices for players to make. For this game, we implemented a very simple map with some gravity fields to show off how it would work. Highlighting the verticality and fast paced nature of the game, as being able to move around quickly and using the gravity fields to your advantage adds a lot more varied choices and playstyle while not adding an additional button/movement mechanic to the player.

![[2.png]]

Our implementations also have a multitude of benefits that are not related to our projects but could be used for other projects especially the wave function collapse as that algorithm can be applied in a variety of different ways including terrain generation as we have implemented. Additionally, this also applies to combining them both as our implementation lacks further procedural generation that allows for the WFC to generate the gravity fields automatically making it truly an isolated system or for making an even more interesting and unique map that the player can explore using the gravity fields and WFC. With structures on a much more bigger scale, it can be very flexible on how you can use it, for example, having a larger WFC structure to create a more cooperative experience where you could have AI that also interacts with the GF’s for a very different and interesting shooter that takes full advantage of the terrain. Implementing additional movement mechanics like wall climbing, jumping, running, double jumping etc. would also be immensely beneficial for this type of structure as the tight pipes/corridors allows for easy access to the walls with the gravity fields also allowing for continuous movement off the ground creating an exciting and thrilling experience. Finally, the system could also be used for other game types such as physics/movement based puzzle games as the gravity fields are able to interact with non-player objects which could allow for interesting ways to the player to explore and fully exploit the gravity fields and their mechanic by changing their direction in real-time or by changing their size.

Overall, our system when combined, does really only have uses when discussing 3D shooters and such, the individual systems themselves can be redefined and recombined to change their purpose and be used for other problems.

## Implementing Multiplayer

The multiplayer implementation of WFC was problematic and ended up taking far longer than I expected. My first approach was to generate the structure on the server and then replicate it down to the client. Unfortunately, I had initially hoped to use a 9x9x9 structure for the map, which meant 2,662 actors needed to be replicated to every client. This was very slow, taking several minutes with multiple connected players.

My next idea was to only replicate the seed for the structure, and then perform the generation on the server and clients individually. This performed much faster but yielded some strange results when walking on the generated terrain. Unless an actor exists as part of the initial level or is replicated to the clients from the server, Unreal doesn't recognise it as valid terrain, even if it is identical on all hosts. Attempting to walk on this terrain has the effect of constantly generating warnings and periodically throwing the character off the map. Looking online, this turned out to be a fairly common problem, but without any clear solutions.

At this point I had two options, either speed up replication or find some way to link actors so that they can be referenced over the network. I decided to start with the first option, since there were quite a few improvements I could make to reduce network load. The first and easiest solution was to simply generate a smaller structure. I chose 3x3x3, cutting the number of actors down to 550. However, even this was sometimes too slow, and the character would sometimes fall through the map before it had fully replicated.

This led me to my current approach, which combines the two ideas described above. When the client starts, it first generates a structure using whatever seed it currently has. Then, if the seed replication happens after the initial generation, it will generate again, this time using the correct seed. Finally, the server replicates the “true” structure to the client, so that walking around on the structure becomes possible. This three step process ensures that the client always has a structure to work with, even if the network is slow, and prevents a lot of the visual artifacts of the slow replication process.

```cpp
// WFCStructure.h

UFUNCTION(Server, Reliable)
void ServerGenerate();

UFUNCTION()
void Generate();

UPROPERTY(ReplicatedUsing = Generate)
int32 Seed;

// WFCStructure.cpp

void AWFCStructure::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
		ServerGenerate();
	else
		Generate();
}

void AWFCStructure::ServerGenerate_Implementation()
{
	Seed = FMath::Rand();
	
	Generate();
}

void AWFCStructure::Generate()
{
	FMath::RandInit(Seed);
	
	// ...
}
```

The next problem was the GridCells. I had made each cell an actor, responsible for holding a space, spawning tiles and checking the adjacency rules of its neighbours. Hence, each tile was a child of a GridCell, which in turn were children of the structure itself. This doubled the number of actors that needed to be replicated, and also created a problem where if a tile was replicated before its parent GridCell, then the tile would always appear in the center of the structure, only moving to its correct position once the GridCell had also been replicated. I fixed this by instead having the GridCells inherit from UObject, making them a logical construct instead of physical. All of the functionality of the GridCells remained the same, however the tiles are now direct children of the structure. This cut the number of replicated actors down to 125.

One final optimization was to prevent the replication of empty tiles. The “clean borders” option creates a shell of empty tiles around the structure before it generates, preventing tiles that attempt to connect to empty space beyond the grid. I considered rewriting this border effect in a way that wouldn't need an extra layer of tiles, and looked at ways of preventing the replication of the empty tile blueprint specifically. In the end, I realised that there was no point in even generating what were essentially empty actors in the first place. By adding a simple check to the CreateTile function, I cut the number of actors down to just 27 for a 3x3x3 structure.

```cpp
// GridCell.cpp

void UGridCell::CreateTile(TSubclassOf TileTypeToSpawn)
{
	// ...
	
	// Prevent spawning of empty tiles
	if (TileTypeToSpawn == EmptyTile)
		return;
		
	Tile = World->SpawnActor(
		TileTypeToSpawn,
		WorldPosition,
		FRotator::ZeroRotator
	);
	
	// ...
}
```

## Assessment 4 Contributions

### Jack

The majority of my time on this project was spent trying to get runtime procedural generation working, as covered in-depth in the previous section. I’m sure I put too much time into this compared to the rest of the project, but there’s nothing more compelling than knowing something *should* be technically possible.

After getting runtime generation working, I spent the remaining time fine-tuning the game feel. I tweaked the character movement script, increasing the jump height to allow the player to traverse gaps in the WFC structure, and greatly increasing the amount of control the player has over their movement while in the air. I also worked on the lighting and materials for the WFC structure, settling on a futuristic golden aesthetic which I think works quite nicely.

### Francisco

My contribution for the time was working on the initial implementation of both the WFC and gravity fields as we realised that it was much harder to import two projects together.

Then for the project, I was responsible for most of the polish and level where I added a kills counter and worked on trying to get the deaths counter (however ran out of time). With the kills counter being continuous even after the player respawns.

I also implemented and fixed up the materials for the gravity fields to make sure that they looked a little nicer and that you can see outside of the gravity field when you are in it.
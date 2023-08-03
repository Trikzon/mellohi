@tool  # Autoloaded Singleton
extends Node


var _blocks: Array[Block] = []
var _id_to_index = Dictionary()  # Dictionary[String, int]


func invalidate():
    _blocks = []
    _id_to_index = Dictionary()
    BlockMaterials._invalidate()
    

func add_block(block: Block):
    if not block:
        push_warning("Attempted to register null block.")
        return
    
    if not block.id or block.id.is_empty():
        push_warning("Attempted to register block with missing id.")
        return
    
    var id = _sanitize_id(block.id)
    
    if _id_to_index.has(id):
        push_warning("Attempting to register block with duplicate id: " + str(id))
        return
    
    block.index = _blocks.size()
    _blocks.append(block)
    _id_to_index[id] = block.index
    
    BlockMaterials._add_block(block)
    
    print("Registered ", block.id, " block with index of ", block.index)


func _sanitize_id(id: String) -> String:
    return id.to_lower().strip_edges()


func id_to_index(id: String) -> int:
    return _id_to_index.get(_sanitize_id(id), 0)


func id_to_block(id: String) -> Block:
    return _blocks[_id_to_index[_sanitize_id(id)]]


func index_to_block(index: int) -> Block:
    return _blocks[index]


func get_block_count() -> int:
    return _blocks.size()

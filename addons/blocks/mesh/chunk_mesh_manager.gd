@tool  # Autoloaded Singleton
extends Node

### Variables that are protected by the mutex.
var _mutex = Mutex.new()
var _exit_thread = false
# It is possible for the chunks to be freed. Should be checked with is_instance_valid before use.
var _registered_chunks = Dictionary()  # Dictionary[Vector3i, ChunkInstance]
var _chunks_to_process_queue: Array[Chunk] = []
var _processed_chunks = Dictionary()  # Dictionary[Vector3i, Chunk]

### Thread-safe variables
var _semaphore = Semaphore.new()
var _thread = Thread.new()


func _ready():
    _thread.start(_thread_process)


func _thread_process():
    while true:
        _semaphore.wait()
        
        _mutex.lock()
        var should_exit = _exit_thread
        _mutex.unlock()
        
        if should_exit: break
            
        _mutex.lock()
        var chunk: Chunk = _chunks_to_process_queue.pop_front()
        if not chunk:
            _mutex.unlock()
            continue
        
        var right = _processed_chunks.get(chunk.chunk_position() + Vector3i.RIGHT)
        var left = _processed_chunks.get(chunk.chunk_position() + Vector3i.LEFT)
        var up = _processed_chunks.get(chunk.chunk_position() + Vector3i.UP)
        var down = _processed_chunks.get(chunk.chunk_position() + Vector3i.DOWN)
        var forward = _processed_chunks.get(chunk.chunk_position() + Vector3i.FORWARD)
        var back = _processed_chunks.get(chunk.chunk_position() + Vector3i.BACK)
        _mutex.unlock()
        
        _generate_chunk(chunk)
        
        # TODO: Only re-generate neighboring chunks if block changed on border OR new chunk
        _generate_chunk(right)
        _generate_chunk(left)
        _generate_chunk(up)
        _generate_chunk(down)
        _generate_chunk(forward)
        _generate_chunk(back)
        


func _generate_chunk(chunk: Chunk):
    if not chunk: return
    
    _mutex.lock()
    var right = _processed_chunks.get(chunk.chunk_position() + Vector3i.RIGHT)
    var left = _processed_chunks.get(chunk.chunk_position() + Vector3i.LEFT)
    var up = _processed_chunks.get(chunk.chunk_position() + Vector3i.UP)
    var down = _processed_chunks.get(chunk.chunk_position() + Vector3i.DOWN)
    var forward = _processed_chunks.get(chunk.chunk_position() + Vector3i.FORWARD)
    var back = _processed_chunks.get(chunk.chunk_position() + Vector3i.BACK)
    _mutex.unlock()
    
    var new_mesh = ChunkMesh.new(chunk)
    new_mesh.generate(right, left, up, down, forward, back)
    var new_shape = new_mesh.create_trimesh_shape()
    
    _mutex.lock()
    var chunk_instance: ChunkInstance = _registered_chunks.get(chunk.chunk_position())
    
    if chunk_instance and is_instance_valid(chunk_instance):
        chunk_instance.call_deferred("set_chunk_mesh", new_mesh, new_shape)
        
        _processed_chunks[chunk.chunk_position()] = chunk
    
    _mutex.unlock()


func register_chunk(chunk_pos: Vector3i, chunk_instance: ChunkInstance):
    _mutex.lock()
    _registered_chunks[chunk_pos] = chunk_instance
    if not chunk_instance.chunk_data_changed.is_connected(_on_chunk_date_changed):
        chunk_instance.chunk_data_changed.connect(_on_chunk_date_changed)
    _mutex.unlock()


func unregister_chunk(chunk_pos: Vector3i):
    _mutex.lock()
    if not _registered_chunks.erase(chunk_pos):
        push_warning("Attempted to unregister chunk from ChunkMeshManager that was not registered at " + str(chunk_pos))
    _processed_chunks.erase(chunk_pos)
    _mutex.unlock()


func _on_chunk_date_changed(chunk: Chunk):
    _mutex.lock()
    _chunks_to_process_queue.push_back(chunk.clone())
    _mutex.unlock()
    
    _semaphore.post()


# Note: print statements should not be run in _exit_tree until this issue is fixed:
# https://github.com/godotengine/godot/issues/80152
func _exit_tree():
    _mutex.lock()
    _exit_thread = true
    _mutex.unlock()
    
    _semaphore.post()
    
    _thread.wait_to_finish()

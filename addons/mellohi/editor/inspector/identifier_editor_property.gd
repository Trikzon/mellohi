extends EditorProperty

var mod_name_line_edit: LineEdit = LineEdit.new()
var address_line_edit: LineEdit = LineEdit.new()

var current_id: Identifier = Identifier.new()


func _init() -> void:
    var hbox = HBoxContainer.new()
    add_child(hbox)

    mod_name_line_edit.size_flags_horizontal = Control.SIZE_EXPAND_FILL
    mod_name_line_edit.placeholder_text = "mod_name"
    mod_name_line_edit.tooltip_text = "Mod Name"
    hbox.add_child(mod_name_line_edit)
    add_focusable(mod_name_line_edit)

    var colon = Label.new()
    colon.text = ":"
    hbox.add_child(colon)

    address_line_edit.size_flags_horizontal = Control.SIZE_EXPAND_FILL
    address_line_edit.placeholder_text = "address"
    address_line_edit.tooltip_text = "Address"
    hbox.add_child(address_line_edit)
    add_focusable(address_line_edit)

    mod_name_line_edit.text_changed.connect(_on_mod_name_text_changed)
    address_line_edit.text_changed.connect(_on_address_text_changed)


func _update_property() -> void:
    var id: Identifier = get_edited_object()[get_edited_property()]

    if not id:
        return
    
    if id.mod_name != mod_name_line_edit.text:
        var caret_column = mod_name_line_edit.caret_column
        mod_name_line_edit.text = id.mod_name
        mod_name_line_edit.caret_column = caret_column
    if id.address != address_line_edit.text:
        var caret_column = address_line_edit.caret_column
        address_line_edit.text = id.address
        address_line_edit.caret_column = caret_column

    current_id = id


func _on_mod_name_text_changed(new_text: String):
    current_id.mod_name = new_text
    emit_changed(get_edited_property(), current_id)


func _on_address_text_changed(new_text: String):
    current_id.address = new_text
    emit_changed(get_edited_property(), current_id)

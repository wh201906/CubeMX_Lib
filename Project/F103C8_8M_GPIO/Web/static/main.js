function onSwitchChanged(id) {
    var currID = $(this).attr('id');
    if ($('#' + id).prop('checked')) {
        console.log("打开", id);
        $.post('/switch', { 'id': id, 'state': '1' });
    } else {
        console.log("关闭", id);
        $.post('/switch', { 'id': id, 'state': '0' });
    }
    return;
}


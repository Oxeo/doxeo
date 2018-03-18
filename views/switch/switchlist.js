<script src="../assets/jquery-ui/jquery-ui.min.js" type="text/javascript"></script>
<script src="../assets/jtable/jquery.jtable.js" type="text/javascript"></script>

<script>
$(document).ready(function () {

    $('#SwitchListContainer').jtable({
        title: 'Switch',
        sorting: false,
        defaultSorting: 'order ASC',
        ajaxSettings: {
            type: 'GET',
            dataType: 'json'
        },
        actions: {
            listAction: 'switch_list.js',
            createAction: 'create_switch.js',
            updateAction: 'edit_switch.js',
            deleteAction: 'delete_switch.js'
        },
        fields: {
            id: {
                title: 'Id',
                key: true,
                create: true,
                edit: false,
                list: true
            },
            name: {
                title: 'Name',
            },
            category: {
                title: 'Category',
                options: { 'light': 'Light', 'socket': 'Socket', 'camera': 'Camera', 'alarm': 'Alarm', 'speaker': 'Speaker', 'fountain': 'Fountain', 'other': 'Other'}
            },
            order: {
                title: 'Order',
            },
            power_on_cmd: {
                title: 'Power on command',
            },
			power_off_cmd: {
                title: 'Power off command',
            },
            status: {
                title: 'Status',
                options: { 'on': 'On', 'off': 'Off'}
            }
        }
    });

    //Load list from server
    $('#SwitchListContainer').jtable('load');

});
</script>
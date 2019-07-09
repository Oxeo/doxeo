<script src="../assets/jquery-ui/jquery-ui.min.js" type="text/javascript"></script>
<script src="../assets/jtable/jquery.jtable.js" type="text/javascript"></script>

<script>
$(document).ready(function () {

    $('#crudContainer').jtable({
        title: 'Switch',
        sorting: false,
        defaultSorting: 'order ASC',
        ajaxSettings: {
            type: 'GET',
            dataType: 'json'
        },
        actions: {
            listAction: 'list.js',
            createAction: 'create.js',
            updateAction: 'update.js',
            deleteAction: 'delete.js'
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
                options: { 'light': 'Light', 'socket': 'Socket', 'camera': 'Camera', 'alarm': 'Alarm', 'speaker': 'Speaker', 'fountain': 'Fountain', 'dog': 'Dog', 'other': 'Other'}
            },
            order: {
                title: 'Order',
            },
            power_on_cmd: {
                title: 'Power on command',
                type: 'textarea'
            },
			power_off_cmd: {
                title: 'Power off command',
                type: 'textarea'
            },
            sensor: {
                title: 'Sensor',
            },
            is_visible: {
                title: 'Visible',
                options: {'true': 'Yes', 'false': 'No'}
            },
            status: {
                title: 'Status',
                options: { 'on': 'On', 'off': 'Off'}
            }
        }
    });

    //Load list from server
    $('#crudContainer').jtable('load');

});
</script>
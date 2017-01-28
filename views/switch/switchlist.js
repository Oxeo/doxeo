<script src="../assets/jquery-ui/jquery-ui.min.js" type="text/javascript"></script>
<script src="../assets/jtable/jquery.jtable.js" type="text/javascript"></script>

<script>
$(document).ready(function () {

    $('#SwitchListContainer').jtable({
        title: 'Switch',
        sorting: false,
        defaultSorting: 'id ASC',
        ajaxSettings: {
            type: 'GET',
            dataType: 'json'
        },
        actions: {
            listAction: 'switch_list.js',
            createAction: 'edit_switch.js',
            updateAction: 'edit_switch.js',
            deleteAction: 'delete_switch.js'
        },
        fields: {
            id: {
                key: true,
                create: false,
                edit: false,
                list: false
            },
            name: {
                title: 'Name',
            },
            command: {
                title: 'Command',
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
<script src="../assets/jquery-ui/jquery-ui.min.js" type="text/javascript"></script>
<script src="../assets/jtable/jquery.jtable.js" type="text/javascript"></script>

<script>
$(document).ready(function () {

    $('#crudContainer').jtable({
        title: 'Heater',
        sorting: false,
        defaultSorting: 'id ASC',
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
                create: false,
                edit: false,
                list: true
            },
            name: {
                title: 'Name'
            },
            power_on_cmd: {
                title: 'Power on cmd'
            },
            power_off_cmd: {
                title: 'Power off cmd'
            },
            mode: {
                title: 'Mode',
                options: { 'Off': 'Off', 'Auto': 'Auto', 'Cool': 'Cool', 'Heat': 'Heat'}
            },
            cool_setpoint: {
                title: 'Cool set point'
            },
            heat_setpoint: {
                title: 'Heat set point'
            },
            sensor: {
                title: 'Sensor'
            }
        }
    });

    //Load list from server
    $('#crudContainer').jtable('load');

});
</script>
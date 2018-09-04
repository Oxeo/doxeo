<script src="../assets/jquery-ui/jquery-ui.min.js" type="text/javascript"></script>
<script src="../assets/jtable/jquery.jtable.js" type="text/javascript"></script>

<script>
$(document).ready(function () {

    $('#SensorListContainer').jtable({
        title: 'Sensor',
        sorting: false,
        defaultSorting: 'id ASC',
        ajaxSettings: {
            type: 'GET',
            dataType: 'json'
        },
        actions: {
            listAction: 'sensor_list.js',
            createAction: 'create_sensor.js',
            updateAction: 'edit_sensor.js',
            deleteAction: 'delete_sensor.js'
        },
        fields: {
            id: {
				title: 'Id',
                key: true,
                create: true,
                edit: false,
                list: true
            },
			cmd: {
				title: 'Command',
            },
            name: {
                title: 'Name',
            },
            value: {
                title: 'Value',
                edit: false
            },
            category: {
                title: 'Category',
                options: { 'temperature': 'Temperature', 'speaker': 'Speaker', 'door': 'Door', 'pump': 'Pump', 'light': 'Light', 'other': 'Other'}
            }
        }
    });

    //Load list from server
    $('#SensorListContainer').jtable('load');

});
</script>
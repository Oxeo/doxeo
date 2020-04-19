<script src="../assets/jquery-ui/jquery-ui.min.js" type="text/javascript"></script>
<script src="../assets/jtable/jquery.jtable.js" type="text/javascript"></script>

<script>
$(document).ready(function () {

    $('#crudContainer').jtable({
        title: 'Sensor',
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
                create: true,
                edit: true,
                list: true
            },
			cmd: {
				title: 'Command',
                type: 'textarea'
            },
            name: {
                title: 'Name',
            },
            full_name: {
                title: 'Full name',
            },
            value: {
                title: 'Value',
                edit: true
            },
            category: {
                title: 'Category',
                options: {'temperature': 'Temperature', 'humidity': 'Humidity', 'speaker': 'Speaker', 'door': 'Door', 'window': 'Window', 'pump': 'Pump', 'light': 'Light', 'doormat': 'Doormat', 'doorknob': 'Doorknob', 'pir': 'PIR', 'plant': 'Plant', 'heartbeat': 'HeartBeat', 'other': 'Other'}
            },
            order: {
                title: 'Order',
            },
            visibility: {
                title: 'Visibility',
                options: { 'hide': 'Hide', 'dashboard': 'Dashboard', 'mobile': 'Mobile' }
            },
            invert_binary: {
                title: 'Invert',
                options: { 'false': 'No', 'true': 'Yes'}
            }
        }
    });

    //Load list from server
    $('#crudContainer').jtable('load');

});
</script>
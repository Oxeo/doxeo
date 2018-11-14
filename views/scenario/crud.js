<script src="../assets/jquery-ui/jquery-ui.min.js" type="text/javascript"></script>
<script src="../assets/jtable/jquery.jtable.js" type="text/javascript"></script>

<script>
$(document).ready(function () {

    $('#crudContainer').jtable({
        title: 'Scenario',
        sorting: true,
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
                edit: false,
                list: true
            },
            name: {
                title: 'Name',
            },
            description: {
                title: 'Description',
            },
            content: {
                title: 'Content',
				type: 'textarea',
				list: false,
            },
            status: {
                title: 'Status',
                options: { 'stop': 'Stop', 'start': 'Start'}
            },
            order: {
                title: 'Order',
            },
            hide: {
                title: 'Hide',
                options: { 'false': 'No', 'true': 'Yes'}
            }
        }
    });

    //Load list from server
    $('#crudContainer').jtable('load');

});
</script>
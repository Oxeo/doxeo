<script src="../assets/jquery-ui/jquery-ui.min.js" type="text/javascript"></script>
<script src="../assets/jtable/jquery.jtable.js" type="text/javascript"></script>

<script>
$(document).ready(function () {

    $('#crudContainer').jtable({
        title: 'Camera',
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
                title: 'Name',
            },
            url: {
                title: 'URL',
            }
        }
    });

    //Load list from server
    $('#crudContainer').jtable('load');

});
</script>
<script src="../assets/jquery-ui/jquery-ui.min.js" type="text/javascript"></script>
<script src="../assets/jtable/jquery.jtable.js" type="text/javascript"></script>

<script>
$(document).ready(function () {

    $('#ScriptListContainer').jtable({
        title: 'Script',
        sorting: false,
        defaultSorting: 'id ASC',
        ajaxSettings: {
            type: 'GET',
            dataType: 'json'
        },
        actions: {
            listAction: 'script_list.js',
            createAction: 'edit_script.js',
            updateAction: 'edit_script.js',
            deleteAction: 'delete_script.js'
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
                options: { 'on': 'On', 'off': 'Off'}
            }
        }
    });

    //Load list from server
    $('#ScriptListContainer').jtable('load');

});
</script>
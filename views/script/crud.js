<script src="../assets/jquery-ui/jquery-ui.min.js" type="text/javascript"></script>
<script src="../assets/jtable/jquery.jtable.js" type="text/javascript"></script>

<script>
$(document).ready(function () {

    $('#crudContainer').jtable({
        title: 'Script',
        sorting: false,
        defaultSorting: 'id ASC',
        ajaxSettings: {
            type: 'GET',
            dataType: 'json'
        },
        toolbar: {
             items: [{                          
                text: 'Export',
                click: function () {
                    window.location.href = 'export';
                }
            }]
        },
        actions: {
            listAction: 'list.js',
            createAction: 'create.js',
            updateAction: 'update.js',
            deleteAction: 'delete.js'
        },
        fields: {
            name: {
                title: 'Name',
            },
            description: {
                title: 'Description',
            },
            visibility: {
                title: 'Visibility',
                options: { 'hide': 'Hide', 'dashboard': 'Dashboard', 'mobile': 'Mobile' }
            },
			content: {
                title: 'Content',
				type: 'hidden',
				list: false,
            },
            status: {
                title: 'Status',
                options: { 'on': 'On', 'off': 'Off'}
            },
            editorButton: {
                title: 'Content',
                display: function(data) {
                     return '<a href="editor?id=' + data.record.id + '"><button type="button" onclick="">Edit</button></a>';
                },
                edit: false
            },
            id: {
                title: 'Id',
                key: true,
                create: false,
                edit: false,
                list: true
            }
        }
    });

    //Load list from server
    $('#crudContainer').jtable('load');

});
</script>
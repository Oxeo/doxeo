<script src="../assets/jquery-ui/jquery-ui.min.js" type="text/javascript"></script>
<script src="../assets/jtable/jquery.jtable.js" type="text/javascript"></script>

<script>
$(document).ready(function () {

    $('#SwitchEventsContainer').jtable({
        title: 'Switch events',
        sorting: false,
        defaultSorting: 'id ASC',
        ajaxSettings: {
            type: 'GET',
            dataType: 'json'
        },
        actions: {
            listAction: 'event_list.js',
            createAction: 'edit_event.js',
            updateAction: 'edit_event.js',
            deleteAction: 'delete_event.js'
        },
        fields: {
            id: {
                key: true,
                create: false,
                edit: false,
                list: false
            },
            switch_id: {
                title: 'Switch',
                width: '20%',
                options: 'switch_options.js'
            },
            day_of_week: {
                title: 'Day',
                width: '20%',
                options: { '1': 'Monday', '2': 'Tuesday', '3': 'Wednesday', '4': 'Thursday', '5': 'Friday', '6': 'Saturday', '7': 'Sunday'},
            },
            time: {
                title: 'Time',
                width: '20%',
            },
            check_freebox: {
                title: 'Check Freebox',
                width: '10%',
                options: { '0': 'No', '1': 'Start', '2': 'Stop', '3': 'Start/Stop', '4': 'Reverse Start', '5': 'Reverse Stop', '6': 'Reverse Start/Stop'}
            }
        }
    });

    //Load list from server
    $('#SwitchEventsContainer').jtable('load');

});
</script>
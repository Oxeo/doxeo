<script type="text/javascript">

jQuery(document).ready(function() {
    $.getJSON('js_user_info', function(result) {
        if (result.success) {
            $('#username').text(result.username);
        } else {
            $.bootstrapGrowl('<h4>Error!</h4> <p>' + result.msg  + '</p>', {
                type: "danger",
                delay: 0,
                allow_dismiss: true
            });
        }
    });
});

$('#submit_change_password').on('click', function(e) {
    var oldPassword = $('#old_password').val();
    var newPassword = $('#new_password').val();
    var newPassword2 = $('#new_password2').val();
    
    if (newPassword !== newPassword2) {
      $('#msgFormChangePassword').text("The two password are not the same!");
    } else {
      changePassword(oldPassword, newPassword);
    }
});

function changePassword(oldPassword, newPassword) {
    var param = {
        old_password: oldPassword,
        new_password: newPassword
    };
    
    $('#msgFormChangePassword').text("");

    $.getJSON('js_change_password', param)
        .done(function(result) {
            if (result.success) {
                $('#modalChangePassword').modal('hide');
                $.bootstrapGrowl('<p>Password changed with success</p>', {
                    type: "success",
                    delay: 10000,
                    allow_dismiss: true
                });
            } else {
                $('#msgFormChangePassword').text(result.msg);
            }
        }).fail(function(jqxhr, textStatus, error) {
            $('#msgFormChangePassword').text(error);
    });
};

</script>


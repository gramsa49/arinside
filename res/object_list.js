$('document').ready(function() {
    var checkBoxes = $('#multiFilter input[type="checkbox"]');

    $(".clearable").on('propertychange keyup input paste', 'input.data_field', function(e) {
        if (e.keyCode == 27 /*Escape-Key*/) { $(this).val(''); }
        $(this).stopTime().oneTime(300, function() {
            $("#execFormFilter").click();
        });
    });
    $("#execFormFilter").click(function() {
        if (schemaList != null) {
            var table_name = 'schemaList';
            var table = $('#' + table_name);
            var search = $("#formNameFilter").val().replace(" +", " ").replace(" ", ".*");
            var numSearch = search.search("^\\d+$");
            var matches = 0;
            var allTypeOn = true;
            var allTypeOff = true;
            var schemaType = new Array();
            for (i = 1; i < 6; i++) {
                schemaType[i] = $('#multiFilter input[value="' + i + '"]').attr('checked');
                if (schemaType[i]) { allTypeOff = false; }
                if (!schemaType[i]) { allTypeOn = false; }
            }
            var hasTypeFilter = !(allTypeOff || allTypeOn);
            var hasFilter = (search != null && search.length > 0) || hasTypeFilter;

            $('#' + table_name + ' tbody tr:gt(0)').remove();

            if (hasFilter) {
                $.each(schemaList, function(i) {
                    var r = new RegExp(search, "i");
                    if ((!hasTypeFilter || hasTypeFilter && schemaType[schemaList[i][5]]) && (schemaList[i][1].match(r) || (numSearch == 0 && ("" + schemaList[i][0]) == search))) {
                        matches++;
                        var row = ($("<tr>")
							.append($("<td>")
								.append(getIcon(rootLevel, 1, 0))
								.append($("<a>").attr("href", schemaList[i][8]).text(schemaList[i][1]))
							)
							.append($("<td>").text(schemaList[i][2]))
							.append($("<td>").text(schemaList[i][3]))
							.append($("<td>").text(schemaList[i][4]))
							.append($("<td>").text(ARSchemaType(schemaList[i][5])))
							.append($("<td>").text(schemaList[i][6]))
							.append($("<td>").text(schemaList[i][7]))
						);

                        table.append(row);
                    }
                });
            }
            $('#schemaListFilterResultCount').text((hasFilter ? "showing " + matches + " out of " : ""));
        }
    });
    $("#typeFilterAll").click(function() {
        $('#multiFilter input[type="checkbox"]').each(function() {
            $(this).attr('checked', true);
        });
        $("#execFormFilter").click();
    });
    $("#typeFilterNone").click(function() {
        checkBoxes.each(function() {
            $(this).attr('checked', false);
        });
        $("#execFormFilter").click();
    });
    $("#typeFilterInvert").click(function() {
        checkBoxes.each(function() {
            $(this).attr('checked', !$(this).attr('checked'));
        });
        $("#execFormFilter").click();
    });
    checkBoxes.change(function() {
        $("#execFormFilter").click();
    });

    if ($("#formNameFilter").val() != "") {
        $("#execFormFilter").click();
    };
});

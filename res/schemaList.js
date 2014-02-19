function initSchemaTable() {
    if (schemaList != null) { 
		schemaListObj = new FilterableTable('schemaList', 'formFilter', 'schemaListFilterResultCount');
		schemaListObj.typeFilter = new Array();
		schemaListObj.onHasTypeFilter(function() {
			var allTypeOn = true;
			var allTypeOff = true;
			for (i = 1; i < 6; i++) {
				this.typeFilter[i] = $('#multiFilter input[value="' + i + '"]').attr('checked');
				if (this.typeFilter[i]) { allTypeOff = false; }
				if (!this.typeFilter[i]) { allTypeOn = false; }
			}
			return !(allTypeOff || allTypeOn)			
		})
		.onCheckTypeFilterForRow(function(row) {
			return schemaListObj.typeFilter[row[5]];
		})
		.setNameIndex(1)
		.onCreateHtmlRow(createSchemaRowHtml);
	}
}

function updateSchemaTable() {
    if (schemaListObj != null) { schemaListObj.filterTable(); }
}

function createSchemaRowHtml(data) {
    return ($("<tr>")
        .append($("<td>")
            .append(getIcon(rootLevel, 1, 0))
            .append($("<a>").attr("href", data[8]).text(data[1]))
        )
        .append($("<td>").text(data[2]))
        .append($("<td>").text(data[3]))
		.append($("<td>").text(data[4]))
		.append($("<td>").text(ARSchemaType(data[5])))
		.append($("<td>").text(data[6]))
		.append($("<td>").text(data[7]))
    );
}

$('document').ready(function() {
    var checkBoxes = $('#multiFilter input[type="checkbox"]');

    $(".clearable").on('propertychange keyup input paste', 'input.data_field', function(e) {
        if (e.keyCode == 27 /*Escape-Key*/) { $(this).val(''); }
        $(this).stopTime().oneTime(300, updateSchemaTable);
    });
    $("#formFilter").click(updateSchemaTable);
    $("#typeFilterNone").click(function() {
        checkBoxes.each(function() {
            this.checked = false;
        });
        updateSchemaTable();
    });
    checkBoxes.change(updateSchemaTable);

    initSchemaTable();
    if ($("#formFilter").focus().val() != "" || schemaListObj.hasTypeFilter()) {
        updateSchemaTable();
    };

    $("#formLetterFilter a").click(function() {
        $("#formFilter").val("^" + this.text);
        updateSchemaTable();
        return false;
    });
});

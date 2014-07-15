#include "ARInsideTest.h"
#include "ARParseField.h"

bool isField(const ARParseField& field, int fieldId)
{
	return (field.tag == AR_FIELD && field.u.fieldId == fieldId);
}

bool isStatusHistory(const ARParseField& field, unsigned int usrOrTime, ARULong32 enumValue)
{
	return (field.tag == AR_STAT_HISTORY &&
		field.u.statHistory.userOrTime == usrOrTime &&
		field.u.statHistory.enumVal == enumValue);
}

bool isCurrencyField(const ARParseField& field, int fieldId, int currencyPartType, const char* partCode)
{
	return (field.tag == AR_CURRENCY_FLD &&
		field.u.currencyField->fieldId == fieldId &&
		field.u.currencyField->partTag == currencyPartType &&
		strcmp(field.u.currencyField->currencyCode, partCode) == 0);
}

TEST(ARParseFieldTests, InvalidFieldExample1)
{
	CARParseField parseField("1234abc");
	const ARParseField &result = parseField.getField();

	ASSERT_EQ(0, result.tag);
}

TEST(ARParseFieldTests, InvalidFieldExample2)
{
	CARParseField parseField("1234$");
	const ARParseField &result = parseField.getField();

	ASSERT_EQ(0, result.tag);
}

TEST(ARParseFieldTests, BasicField)
{
	CARParseField parseField("536870920");
	const ARParseField &result = parseField.getField();

	ASSERT_TRUE(isField(result, 536870920));
}

TEST(ARParseFieldTests, StatusHistory)
{
	CARParseField parseField("15.1.3");
	const ARParseField &result = parseField.getField();

	ASSERT_TRUE(isStatusHistory(result, AR_STAT_HISTORY_USER, 3));
}

TEST(ARParseFieldTests, InvalidStatusHistory)
{
	CARParseField parseField("15.4.2");
	const ARParseField &result = parseField.getField();

	ASSERT_EQ(0, result.tag);
}

TEST(ARParseFieldTests, Keyword)
{
	CARParseField parseField("-6");
	const ARParseField &result = parseField.getField();

	ASSERT_TRUE(isField(result, -6));
}

TEST(ARParseFieldTests, InvalidKeyword)
{
	CARParseField parseField("-15.1.3");
	const ARParseField &result = parseField.getField();

	ASSERT_EQ(0, result.tag);
}

TEST(ARParseFieldTests, CurrencyField)
{
	CARParseField parseField("536870987.1.USD");
	const ARParseField &result = parseField.getField();

	ASSERT_TRUE(isCurrencyField(result, 536870987, 1, "USD"));
}
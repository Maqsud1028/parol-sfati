String^ PasswordChecker::GetLeetSpeakFeedback(String^ password)
{
    if (!IsLeetSpeakVariant(password))
        return "";

    String^ normalized = NormalizeLeetSpeak(password);
    
    return "⚠ Leet Speak aniqlandi: \"" + password + 
           "\" → \"" + normalized + "\"\n" +
           "Bu usul zamonaviy hujum vositalariga to'siq bo'la olmaydi.\n" +
           "Tavsiya: Butunlay tasodifiy va lug'atda bo'lmagan paroldan foydalaning.";
}
int PasswordChecker::ApplyLeetSpeakPenalties(
    String^ password, PasswordResult^ result)
{
    int penalty = 0;

    if (!IsLeetSpeakVariant(password))
        return 0;

    result->IsLeetSpeak = true;
    String^ normalized  = NormalizeLeetSpeak(password);
    int     replacements = CountLeetReplacements(password);
    double  density = (double)replacements / password->Length;

    // Holat 1: Zaif so'zning to'liq Leet Speak varianti
    array<String^>^ exactWeakWords = {
        "password", "admin", "parol", "tashkent",
        "uzbekiston", "iloveyou", "qwerty", "letmein"
    };
    for each (String^ word in exactWeakWords)
    {
        if (normalized == word)
        {
            penalty = 40;
            result->Feedback->Add(
                "⚠ Leet Speak aniqlandi: \"" + password +
                "\" → \"" + normalized + "\"\n"
                "  Zamonaviy vositalar bu almashtirishlarni "
                "avtomatik sinab ko'radi.");
            return penalty;
        }
    }

    // Holat 2: Leet Speak + sana naqshi kombinatsiyasi
    if (result->HasPattern && density >= 0.2)
    {
        penalty = 45;
        result->Feedback->Add(
            "⚠ Leet Speak va naqsh kombinatsiyasi aniqlandi. "
            "Bu parol ayniqsa zaif hisoblanadi.");
        return penalty;
    }

    // Holat 3: Qisman moslik (zaif so'z parol ichida)
    if (density >= 0.3)
    {
        penalty = 25;
        result->Feedback->Add(
            "⚠ Parolda Leet Speak almashtirishlari aniqlandi. "
            "Bu usul yetarli himoya ta'minlamaydi.");
    }
    else if (replacements <= 2)
    {
        penalty = 10;
        result->Feedback->Add(
            "ℹ Minimal Leet Speak almashtirishlari topildi.");
    }

    return penalty;
}

bool PasswordChecker::HasCommonPattern(String^ password)
{
    // 1. Ketma-ket raqamlar
    if (Regex::IsMatch(password, "\\d{4,}"))
        return true;
    // 2. Sana naqshi
    if (Regex::IsMatch(password,
        "(0[1-9]|[12][0-9]|3[01])[.\\-/]?(0[1-9]|1[0-2])[.\\-/]?[0-9]{2,4}"))
        return true;
    // 3. Takrorlanuvchi belgilar
    if (Regex::IsMatch(password, "(.+)\\1{2,}"))
        return true;
    // 4. Klaviatura qatori naqshlari
    array<String^>^ keyboardRows = {
        "qwerty", "asdfgh", "zxcvbn",
        "qazwsx", "1qaz2wsx", "yuiop"
    };
    String^ lowerPass = password->ToLower();
    for each (String^ row in keyboardRows)
    {
        if (lowerPass->Contains(row) ||
            lowerPass->Contains(
                gcnew String(System::Linq::Enumerable::Reverse(
                    row->ToCharArray())->ToArray())))
            return true;
    }
    // 5. So'z + raqam (oxirda faqat raqamlar)
    if (Regex::IsMatch(password, "^[a-zA-Z]{3,}\\d{1,4}$"))
        return true;
    return false;
}


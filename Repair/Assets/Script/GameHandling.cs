using System;
using System.Collections;
using System.Linq;
using DefaultNamespace;
using LanguageExt;
using static LanguageExt.Prelude;
using Script;
using UnityEngine;
using Patch = Script.Patch;
using Random = UnityEngine.Random;
using EZCameraShake;
using UnityEngine.Serialization;

public class GameHandling : MonoBehaviour
{
    public GameStatus gs;

    public GameObject invertedSphere;

    public GameObject waterLevel;

    public GameObject[] ecoutilles;

    public GameObject PatchPrefab;

    public AudioSource CameraAudio;

    public AudioSource[] AlertAudio;

    public GameObject TheChair;

    public GameObject Spawner;

    public Animator sub_animator;

    private Renderer _invertedSphereRenderer;

    private Color _originalColor;

    private Option<GameObject> _currentPatch = Option<GameObject>.None;

    private float _thrust = 1f;


    private Color _initialEmissiveSphereColor;

    void Start()
    {
        _invertedSphereRenderer = invertedSphere.GetComponentInChildren<Renderer>();
        // Get the initial color of the inversed sphere.
        HideInvertedSphere();
        _originalColor = _invertedSphereRenderer.material.color;
        _initialEmissiveSphereColor = _invertedSphereRenderer.material.GetColor("_EmissionColor");
        RestartGame();
        Debug.Log("GameHandling Started.");
    }


    #region Map Range definition

    static float MapRange(float x, float a, float b, float min, float max)
    {
        return (((b - a) * (x - min)) / (max - min)) + a;
    }

    static Func<float, float, float, float, float, float> mapRange = MapRange;

    static Func<float, float> mapRangeOf = ApplyPartial(mapRange, 0f, 6f, 0f, 2f);

    static Func<T1, TResult> ApplyPartial<T1, T2, T3, T4, T5, TResult>
        (Func<T1, T2, T3, T4, T5, TResult> f, T2 a, T3 b, T4 min, T5 max)
    {
        return (x) => f(x, a, b, min, max);
    }

    #endregion

    // Update is called once per frame
    void Update()
    {
        var distance = gs.headsetLevel - gs.waterLevel;
        if (distance < 0.1f)
        {
            _invertedSphereRenderer.enabled = true;


            var material = _invertedSphereRenderer.material;
            var newColor = _initialEmissiveSphereColor * mapRangeOf(-distance);
            material.SetColor("_EmissionColor", newColor);
            if (_gameoverCoroutine == null)
                _gameoverCoroutine = StartCoroutine(GameOverControl());
        }
        else
        {
            var material = _invertedSphereRenderer.material;
            material.SetColor("_EmissionColor", (_initialEmissiveSphereColor));
            if (_gameoverCoroutine != null)
            {
                StopCoroutine(_gameoverCoroutine);
                _gameoverCoroutine = null;
            }

            HideInvertedSphere();
        }
    }

    public void spawnAPatch()
    {
        // _currentPatch.Filter(x => !x.GetComponent<Script.Patch>().isGrabbed())
        //     .Map(x => x.GetComponent<Script.Patch>().ChangeShape());

        Debug.LogFormat("The current patch exists:  {0}", _currentPatch.IsNone);
        // _currentPatch.IfNone(() => UnityThread.executeInUpdate(() => createNewPatch()));
        UnityThread.executeInUpdate(() => createNewPatch());
    }

    private void createNewPatch()
    {
        if (_currentPatch.IsSome) return;

        Debug.Log("Create a new Instance of the prefab");
        try
        {
            var newPatch = Instantiate(PatchPrefab, Spawner.transform);
            newPatch.GetComponentInChildren<Script.Patch>().SetGameHandling(this.gameObject);
            // newPatch.GetComponentInChildren<Rigidbody>().AddForce(transform.forward * _thrust);
            _currentPatch = newPatch;
        }
        catch (Exception e)
        {
            Debug.LogFormat("Cannot create an instance of the prefab : {0}", e.Message);
        }
    }

    /**
     * This is gameover, close everything and stop everything
     */
    private void ApplyGameOver()
    {
        Debug.Log("GameOver");
        gs.gameover = true;
        if (_waterLevelCoroutine != null) StopCoroutine(_waterLevelCoroutine);
        if (_failureGeneratorCoroutine != null) StopCoroutine(_failureGeneratorCoroutine);
        _invertedSphereRenderer.material.color = new Color(0, 0, 0, 1f);
        _invertedSphereRenderer.material.DisableKeyword("_EMISSION");
    }

    /**
     * A new game is starting setup everything and let's play...
     */
    public void RestartGame()
    {
        waterLevel.transform.position = Vector3.zero;
        if (_invertedSphereRenderer != null) _invertedSphereRenderer.material.color = _originalColor;
        HideInvertedSphere();
        _waterLevelCoroutine = StartCoroutine(WaterLevelControl());
        _failureGeneratorCoroutine = StartCoroutine(FailureGeneratorControl());
        _initialAverageCoroutine = StartCoroutine(InitalAvarage());
        if (_gameoverCoroutine != null) StopCoroutine(_gameoverCoroutine);
        _gameoverCoroutine = null;
        gs.gameover = false;
        FixAllHoles();
    }

    private void HideInvertedSphere()
    {
        if (_invertedSphereRenderer == null) return;
        _invertedSphereRenderer.enabled = false;
    }

    private void crash()
    {
        Debug.Log("Crash!!...");
        CameraAudio.Play();
        StartCoroutine(atImpact(3f));
    }

    private IEnumerator atImpact(float time)
    {
        yield return new WaitForSecondsRealtime(time);
        foreach (var audioSource in AlertAudio)
        {
            audioSource.Play();
        }

        sub_animator.SetTrigger("crash");
        sub_animator.SetBool("isAlert", true);
        GenerateAverage();
        _PatchSpanerCoroutine = StartCoroutine(PatchSpawner());
    }

    private void FixAllHoles()
    {
        Debug.Log("FixHoles...");
        foreach (GameObject ecoutille in ecoutilles)
        {
            ecoutille.GetComponent<Ecoutille>().fixIt();
        }
    }

    private void GenerateAverage()
    {
        Debug.Log("Generate Average...");
        foreach (GameObject ecoutille in ecoutilles)
        {
            if (Random.Range(0f, 1f) > .7f)
            {
                Debug.LogFormat("Damaging '{0}'...", ecoutille.name);
                ecoutille.GetComponent<Ecoutille>().brokeIt();
            }
        }
    }

    /*
     * Get the number of broken holes
     */
    private int NumberOfBrokenHole()
    {
        return ecoutilles.Where(x => x.GetComponent<Ecoutille>().isBroken()).Count();
    }

    /**
     * Clean up the GameStatus Singleton resource
     */
    private void OnDestroy()
    {
        gs.gameover = false;
    }

    #region Timers

    private Coroutine _fadeCoroutine;
    private Coroutine _gameoverCoroutine;
    private Coroutine _waterLevelCoroutine;
    private Coroutine _failureGeneratorCoroutine;
    private Coroutine _initialAverageCoroutine;
    private Coroutine _PatchSpanerCoroutine;


    /**
     * Generate the initial average
     */
    private IEnumerator InitalAvarage()
    {
        yield return new WaitForSeconds(4f);
        crash();
    }

    /**
     * Control the level of the water depending the number of active holes
    */
    private IEnumerator WaterLevelControl()
    {
        for (;;)
        {
            yield return new WaitForSeconds(gs.waterLevelCheckIntervalInSeconds);
            // Debug.LogFormat("WATER LEVEL: {0}", gs.waterLevel);
            var transformPosition = waterLevel.transform.position;

            transformPosition.y = transformPosition.y + gs.waterLevelSpeed * NumberOfBrokenHole() * .001f;
            waterLevel.transform.position = transformPosition;
        }
    }

    private IEnumerator PatchSpawner()
    {
        for (;;)
        {
            yield return new WaitForSeconds(1);
            UnityThread.executeInUpdate(() => createNewPatch());
        }
    }

    /**
     * Create failure randomly
     */
    private IEnumerator FailureGeneratorControl()
    {
        for (;;)
        {
            yield return new WaitForSeconds(gs.FailureGeneratorIntervalInSecond);
            Debug.LogFormat("Let's Break some Holes....");
            GenerateAverage();
        }
    }

    /**
     * Control the time allowed underwater, once reached, it's gameover, that's life!
     */
    private IEnumerator GameOverControl()
    {
        for (int i = 0; i < gs.underWaterSecondBeforeGameOver * 10; i++)
        {
            Debug.LogFormat("GameOver: iter({0})", i);
            yield return new WaitForSeconds(.1f);
        }

        Debug.Log("APPLY GAMEOVER");
        ApplyGameOver();
    }

    IEnumerator Fade()
    {
        if (_invertedSphereRenderer == null) yield break;
        _invertedSphereRenderer.enabled = true;

        for (float ft = 0f; ft <= 1; ft += 0.1f)
        {
            var material = _invertedSphereRenderer.material;
            Color c = material.color;
            c.a = ft;
            material.color = c;
            yield return new WaitForSeconds(.1f);
        }
    }

    #endregion

    public void PatchMatched(Ecoutille ecoutille)
    {
        Debug.Log("Patch matched, fix the ecoutille and destroy the path .....");
        _currentPatch.IfSome(p => Destroy(p));
        _currentPatch = Option<GameObject>.None;
        ecoutille.fixIt();
    }
}